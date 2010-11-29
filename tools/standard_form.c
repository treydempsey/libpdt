#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <tgcl.h>
#include <pdt.h>


#define SHELL_TRUE    0
#define SHELL_FALSE   1
#define PDT_BLK_SIZE  8192


/* File Globals */
static PDTFormat *  format;
static String *     format_file;
static String *     input_file;
static String *     output_file;
static int          output_file_fd;
static String *     output_buffer;
static Array *      record;
static int          field_count;
static int          valid;


/* Prototypes */
static void         usage(char * argv0);
static void         set_params(int argc, char * argv[]);
static int          open_input_file(void);
static void         close_input_file(int input_file_fd);
static int          open_output_file(void);
static void         close_output_file(int output_file_fd);
static void         write_output_file(void);
static void         fieldcb(ParseDelimitedText * parser, String * field);
static void         recordcb(ParseDelimitedText * parser, char eol);
static void         error(const char * format, ...);


int
main(int argc, char * argv[], char * envp[])
{
  ParseDelimitedText *  parser;
  int                   input_file_fd;
  char                  input_file_buffer[PDT_BLK_SIZE];
  size_t                bytes_read;

  /* Initalize the classes we're using */
  class_Array();
  class_String();
  class_PDTFormat();
  class_PDTColumn();
  class_PDTFormatFile();
  class_ParseDelimitedText();

  /* Parse command line parameters */
  set_params(argc, argv);
  
  /* Load the format file */
  format = new_PDTFormat();
  format->m->read_file(format, format_file->string, format_file->length);

  /* Open the files */
  input_file_fd = open_input_file();
  output_file_fd = open_output_file();

  /* Set the parser's options */
  parser = new_ParseDelimitedText(0);
  parser->m->apply_format(parser, format);
  parser->m->set_block_size(parser, PDT_BLK_SIZE);
  parser->m->set_field_callback(parser, fieldcb);
  parser->m->set_record_callback(parser, recordcb);

  /* Initialize Globals */
  record = new_Array();
  record->auto_free = null_String->m->Array_free;
  format->columns->m->reset_each(format->columns);
  while(format->columns->m->each(format->columns) != null_ArrayElement) {
    record->m->append(record, new_String("", 0));
  }
  field_count = 0;
  valid = 1;
  output_buffer = new_String("", 0);
  output_buffer->m->extend(output_buffer, 8191);

  /* Convert the file to standard form */
  while(parser->stop == 0
        && (bytes_read = read(input_file_fd, input_file_buffer, PDT_BLK_SIZE)) != 0) {
    parser->m->parse(parser, input_file_buffer, bytes_read);
  }
  parser->m->finish(parser);
  if(output_file->length != 0) {
    write_output_file();
  }

  /* Cleanup */
  close_input_file(input_file_fd);
  close_output_file(output_file_fd);
  parser->m->free(parser);
  record->m->free(record);
  format->m->free(format);
  output_buffer->m->free(output_buffer);

  return (valid) ? SHELL_TRUE : SHELL_FALSE;
}


void
set_params(int argc, char * argv[])
{ /* {{{ */
  struct stat file_stat;

  if(argc <= 2 || argc >= 5) {
    usage(argv[0]);
    exit(SHELL_FALSE);
  }

  /* Format File */
  if(stat(argv[1], &file_stat) == -1) {
    perror("stat() error for format file");
    exit(1);
  }
  if(! S_ISREG(file_stat.st_mode)) {
    fprintf(stderr, "Format file is not a regular file.\n");
    exit(1);
  }
  format_file = new_String(argv[1], strlen(argv[1]));


  /* Input File */
  if(stat(argv[2], &file_stat) == -1) {
    perror("stat() error for input file");
    exit(1);
  }
  if(! S_ISREG(file_stat.st_mode)) {
    fprintf(stderr, "Input file is not a regular file.\n");
    exit(1);
  }
  input_file = new_String(argv[2], strlen(argv[2]));


  /* Output File */
  if(argc == 4
     && strncmp(argv[3], "-", 1) != 0) {
    if(stat(argv[3], &file_stat) == -1) {
      if(errno != ENOENT) {
        perror("stat() error for output file");
        exit(1);
      }
    }
    else if(! S_ISREG(file_stat.st_mode)) {
      fprintf(stderr, "Output file exists, but is not a regular file.\n");
      exit(1);
    }
    output_file = new_String(argv[3], strlen(argv[3]));
  }
  else {
    output_file = new_String("-", 1);
  }

} /* }}} */


static void
usage(char * argv0)
{ /* {{{ */
  printf("\nUsage: %s <format file> <input file> <output file>\n", basename(argv0));
  printf("\n"
         "DESCRIPTION\n"
         "\n"
         "\tTransforms input file, using format file for parsing, into standard form, writing\n"
         "\tthe result to output file.\n"
         "\n"
         "STANDARD FORM\n"
         "\n"
         "\t* The first record is always the header\n"
         "\t* Records end with <LF> 0x1A\n"
         "\t* Fields are delimited with <US> 0x1F\n"
         "\t* New lines within fields are always <CR> 0x0A\n"
         "\n"
         "FORMAT FILE\n"
         "\n"
         "\tDelimiter: <delimiter string, may be multiple characters>\n"
         "\tQuote: <quoting character surrounding fields>\n"
         "\tEscape: <escape character>\n"
         "\tAllow Fewer Columns: <1|0.  Defaults to 0.  Allow less than the total number of columns on a line.>\n"
         "\tColumn List:\n"
         "\t\tcolumn1\n"
         "\t\tcolumn2\n"
         "\t\tEach column must start with whitespace\n"
         "\t\t...\n"
         "\n"
         "\tDelimiter, Quote, and Escape can be literal characters with the exception of whitespace or\n"
         "\thexadecimal ascii in 0x?? format.  Delimiter and column list are required.\n\n"
  );
} /* }}} */


static int
open_input_file(void)
{ /* {{{ */
  int         fd;

  if((fd = open(input_file->string, O_RDONLY)) == -1) {
    perror("Error opening input file");
    exit(1);
  }

  return fd;
} /* }}} */


static void
close_input_file(int input_file_fd)
{ /* {{{ */
  close(input_file_fd);
} /* }}} */


static int
open_output_file(void)
{ /* {{{ */
  int         fd;

  if(strncmp(output_file->string, "-", 1) != 0) {
    if((fd = open(output_file->string, O_WRONLY | O_CREAT, 0644)) == -1) {
      perror("Error opening output file");
      exit(1);
    }
  }
  else {
    fd = fileno(stdout);
  }

  return fd;
} /* }}} */


static void
close_output_file(int output_file_fd)
{ /* {{{ */
  if(strncmp(output_file->string, "-", 1) != 0) {
    close(output_file_fd);
  }
} /* }}} */


static void
write_output_file(void)
{ /* {{{ */
  write(output_file_fd, output_buffer->string, output_buffer->length);
  output_buffer->m->truncate(output_buffer);
} /* }}} */


static void
fieldcb(ParseDelimitedText * parser, String * field)
{ /* {{{ */
  if(field_count <= (record->length - 1)) {
    ((String *)record->elements[field_count]->data)->m->truncate((String *)record->elements[field_count]->data);
    ((String *)record->elements[field_count]->data)->m->append((String *)record->elements[field_count]->data, field);
  }
  field_count++;
} /* }}} */


static void
recordcb(ParseDelimitedText * parser, char eol)
{ /* {{{ */
  if(field_count != record->length) {
    error("Wrong field count on row %zu, found %i expected %i\n", parser->record_count + 1, field_count, record->length);
    valid = 0;
    parser->m->stop(parser);
  }
  else {
    record->m->reset_each(record);
    while(record->m->each(record) != null_ArrayElement) {
      if((output_buffer->length + ((String *)record->current_element->data)->length + 2) >= output_buffer->size) {
        write_output_file();
      }

      output_buffer->m->append(output_buffer, (String *)record->current_element->data);
      if(record->position <= (record->length - 2)) {
        output_buffer->m->append_cstr(output_buffer, "\x1f", 1);
      }
    }
    output_buffer->m->append_cstr(output_buffer, "\x0a", 1);
    field_count = 0;
  }
} /* }}} */


static void
error(const char * format, ...)
{ /* {{{ */
  va_list args;
  char *  err_fmt;

  va_start(args, format);

  err_fmt = malloc((strlen(format) + 8) * sizeof(char));
  strncpy(err_fmt, "ERROR: ", 7);
  strncpy(err_fmt + 7, format, strlen(format));

  vfprintf(stderr, err_fmt, args);

  free(err_fmt);

  va_end(args);
} /* }}} */
