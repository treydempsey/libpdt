#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <tgcl.h>
#include <pdt.h>


typedef struct String_KV {
  String * key;
  String * value;
} String_KV;


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
static Array *      constants;
static int          valid;


/* Prototypes */
static void         set_params(int argc, char * argv[]);
static void         usage(char * argv0);
static Array *      parse_constants(const char * arg);
static int          open_input_file(void);
static void         close_input_file(int input_file_fd);
static int          open_output_file(void);
static void         close_output_file(int output_file_fd);
static void         write_output_file(void);
static void         fieldcb(ParseDelimitedText * parser, String * field);
static void         recordcb(ParseDelimitedText * parser, char eol);
static void         error(const char * format, ...);
static String_KV *  new_String_KV(void);
static void         Array_String_KV_free(void * data);


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
  format->columns->m->reset_each(format->columns);

  /* Open the files */
  input_file_fd = open_input_file();
  output_file_fd = open_output_file();

  /* Set the parser's options */
  parser = new_ParseDelimitedText(0);
  parser->m->apply_format(parser, format);
  parser->m->set_block_size(parser, PDT_BLK_SIZE);
  parser->m->set_delimiter(parser, "\x1f", 1);
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
  output_buffer = new_String("", 0);
  output_buffer->m->extend(output_buffer, 8191);
  valid = 1;

  /* Write the file */
  while(parser->stop == 0
        && (bytes_read = read(input_file_fd, input_file_buffer, PDT_BLK_SIZE)) != 0) {
    parser->m->parse(parser, input_file_buffer, bytes_read);
  }
  parser->m->finish(parser);
  if(output_file->length != 0) {
    write_output_file();
  }

  /* Cleanup Files */
  close_input_file(input_file_fd);
  close_output_file(output_file_fd);

  /* Cleanup Locals */
  parser->m->free(parser);

  /* Cleanup Globals */
  record->m->free(record);
  format->m->free(format);
  output_buffer->m->free(output_buffer);
  constants->m->free(constants);

  return (valid) ? SHELL_TRUE : SHELL_FALSE;
}


void
set_params(int argc, char * argv[])
{ /* {{{ */
  struct stat file_stat;

  if(argc < 4 || argc > 5) {
    usage(argv[0]);
    exit(SHELL_FALSE);
  }

  
  /* Constants */
  constants = parse_constants(argv[1]);

  /* Format File */
  if(stat(argv[2], &file_stat) == -1) {
    perror("ERROR: stat() error for format file");
    exit(SHELL_FALSE);
  }
  if(! S_ISREG(file_stat.st_mode)) {
    error("Format file is not a regular file.\n");
    exit(SHELL_FALSE);
  }
  format_file = new_String(argv[2], strlen(argv[2]));

  /* Input File */
  if(stat(argv[3], &file_stat) == -1) {
    perror("ERROR: stat() error for input file");
    exit(SHELL_FALSE);
  }
  if(! S_ISREG(file_stat.st_mode)) {
    error("Input file is not a regular file.\n");
    exit(SHELL_FALSE);
  }
  input_file = new_String(argv[3], strlen(argv[3]));

  /* Output File */
  if(argc == 5
     && strncmp(argv[4], "-", 1) != 0) {
    if(stat(argv[4], &file_stat) == -1) {
      if(errno != ENOENT) {
        perror("ERROR: stat() error for output file");
        exit(1);
      }
    }
    else if(! S_ISREG(file_stat.st_mode)) {
      error("Output file exists, but is not a regular file.\n");
      exit(SHELL_FALSE);
    }
    output_file = new_String(argv[4], strlen(argv[4]));
  }
  else {
    output_file = new_String("-", 1);
  }

  /* Ensure input_file != output_file */
  if(input_file->m->compare(input_file, output_file) == 0) {
    error("Input and output cannot be the same file\n");
    exit(SHELL_FALSE);
  }
} /* }}} */


static void
usage(char * argv0)
{ /* {{{ */
  printf("\nUsage: %s <name1=constant1,name2=constant2> <format file> <input file> <output file>\n", basename(argv0));
  printf("\n"
         "DESCRIPTION\n"
         "\n"
         "\tAdds constants to the end of each row in input file, using format file for parsing.\n"
         "\tThe result is written to output file.\n\n"
  );
} /* }}} */


static Array *
parse_constants(const char * arg)
{ /* {{{ */
  String *    arg_str;
  String *    comma;
  String *    equal;
  Array *     comma_split;
  Array *     equal_split;
  String_KV * kv;
  Array *     c = null_Array;

  arg_str = new_String((char *)arg, strlen(arg));
  comma = new_String(",", 1);
  equal = new_String("=", 1);

  comma_split = arg_str->m->split(arg_str, comma);
  if(comma_split != null_Array) {
    c = new_Array();
    c->auto_free = Array_String_KV_free;
    comma_split->m->reset_each(comma_split);
    while(comma_split->m->each(comma_split) != null_ArrayElement) {
      equal_split = ((String *)comma_split->current_element->data)->m->split((String *)comma_split->current_element->data, equal);
      if(equal_split != null_Array && equal_split->length == 2) {
        kv = new_String_KV();
        kv->key = ((String *)equal_split->elements[0]->data)->m->dup((String *)equal_split->elements[0]->data);
        kv->value = ((String *)equal_split->elements[1]->data)->m->dup((String *)equal_split->elements[1]->data);
        c->m->append(c, kv);
      }
      else {
        error("Malformed constant: %s\n", ((String *)comma_split->current_element->data)->string);
        exit(SHELL_FALSE);
      }
      equal_split->m->free(equal_split);
    }
    comma_split->m->free(comma_split);
  }

  arg_str->m->free(arg_str);
  comma->m->free(comma);
  equal->m->free(equal);

  return c;
} /* }}} */


static int
open_input_file(void)
{ /* {{{ */
  struct stat file_stat;
  int         fd;

  if(stat(input_file->string, &file_stat) == -1) {
    perror("stat() error for input file");
    exit(1);
  }

  if(! S_ISREG(file_stat.st_mode)) {
    fprintf(stderr, "Input file is not a regular file.\n");
    exit(1);
  }

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
      perror("ERROR: opening output file");
      exit(SHELL_FALSE);
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
      if(record->position <= (record->length + constants->length - 2)) {
        output_buffer->m->append_cstr(output_buffer, "\x1f", 1);
      }
    }

    constants->m->reset_each(constants);
    while(constants->m->each(constants) != null_ArrayElement) {
      /* Header */
      if(parser->record_count == 0) {
        if((output_buffer->length + ((String_KV *)constants->current_element->data)->key->length + 2) >= output_buffer->size) {
          write_output_file();
        }

        output_buffer->m->append(output_buffer, ((String_KV *)constants->current_element->data)->key);
        if(((record->length - 1) + constants->position) < (record->length + constants->length - 2)) {
          output_buffer->m->append_cstr(output_buffer, "\x1f", 1);
        }
      }
      /* Body */
      else {
        if((output_buffer->length + ((String_KV *)constants->current_element->data)->value->length + 2) >= output_buffer->size) {
          write_output_file();
        }

        output_buffer->m->append(output_buffer, ((String_KV *)constants->current_element->data)->value);
        if(((record->length - 1) + constants->position) < (record->length + constants->length - 2)) {
          output_buffer->m->append_cstr(output_buffer, "\x1f", 1);
        }
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


static String_KV *
new_String_KV(void)
{ /* {{{ */
  String_KV * kv;

  kv = malloc(sizeof(String_KV));

  return kv;
} /* }}} */


static void
Array_String_KV_free(void * data)
{ /* {{{ */
  String_KV * kv;

  kv = (String_KV *)data;
  free(kv);
} /* }}} */
