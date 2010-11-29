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
static Array *      header_fields;
static int          field_count;
int                 valid;


/* Prototypes */
static void     usage(char * argv0);
static void     set_params(int argc, char * argv[]);
static int      open_input_file(void);
static int      validate_header(void);
static void     header_field(ParseDelimitedText * parser, String * field);
static void     header_record(ParseDelimitedText * parser, char eol);
static void     field(ParseDelimitedText * parser, String * field);
static void     record(ParseDelimitedText * parser, char eol);
static void     error(const char * format, ...);


int
main(int argc, char * argv[], char * envp[])
{
  ParseDelimitedText *  parser;
  int                   input_file_fd;
  char                  input_file_buffer[PDT_BLK_SIZE];
  size_t                bytes_read;

  /* Parse command line parameters */
  set_params(argc, argv);
  
  /* Load the format file */
  printf("Reading format file %s\n", format_file->string);
  format = new_PDTFormat();
  format->m->read_file(format, format_file->string, format_file->length);
  format->columns->m->reset_each(format->columns);

  /* Open the input file */
  input_file_fd = open_input_file();
  parser = new_ParseDelimitedText(0);

  /* Set the parser's options */
  parser->m->apply_format(parser, format);
  parser->m->set_block_size(parser, PDT_BLK_SIZE);
  parser->m->set_field_callback(parser, header_field);
  parser->m->set_record_callback(parser, header_record);

  /* Initialize Globals */
  header_fields = new_Array();
  header_fields->auto_free = null_String->m->Array_free;
  valid = 1;
  field_count = 0;

  /* Validate the file header */
  while(parser->stop == 0
        && (bytes_read = read(input_file_fd, input_file_buffer, PDT_BLK_SIZE)) != 0) {
    parser->m->parse(parser, input_file_buffer, bytes_read);
  }
  parser->m->finish(parser);
  valid = validate_header();

  /* Validate the file body */
  if(valid == 1) {
    parser->m->set_field_callback(parser, field);
    parser->m->set_record_callback(parser, record);
    parser->stop = 0;
    while(parser->stop == 0
          && (bytes_read = read(input_file_fd, input_file_buffer, PDT_BLK_SIZE)) != 0) {
      parser->m->parse(parser, input_file_buffer, bytes_read);
    }
    parser->m->finish(parser);
  }

  /* Cleanup */
  close(input_file_fd);
  parser->m->free(parser);
  header_fields->m->free(header_fields);
  format->m->free(format);

  return (valid) ? SHELL_TRUE : SHELL_FALSE;
}


void
set_params(int argc, char * argv[])
{ /* {{{ */
  struct stat file_stat;

  if(argc != 3) {
    usage(argv[0]);
    exit(SHELL_FALSE);
  }

  if(stat(argv[1], &file_stat) == -1) {
    perror("stat() error for format file");
    exit(1);
  }

  if(! S_ISREG(file_stat.st_mode)) {
    fprintf(stderr, "Format file is not a regular file.\n");
    exit(1);
  }


  if(stat(argv[2], &file_stat) == -1) {
    perror("stat() error for input file");
    exit(1);
  }

  if(! S_ISREG(file_stat.st_mode)) {
    fprintf(stderr, "Input file is not a regular file.\n");
    exit(1);
  }

  format_file = new_String(argv[1], strlen(argv[1]));
  input_file = new_String(argv[2], strlen(argv[2]));
} /* }}} */


static void
usage(char * argv0)
{ /* {{{ */
  printf("\nUsage: %s <format file> <input file>\n", basename(argv0));
  printf("\n"
         "DESCRIPTION\n"
         "\n"
         "\tValidates each row of input file using format file.  Exits with error code 0 if the file is valid,\n"
         "\tand error code 2 if the file is not valid.\n"
         "\n"
         "VALIDATIONS\n"
         "\n"
         "\t* Check header names, and positions against format file\n"
         "\t* Check each row has the correct number of fields separated by delimiter and optional quoting\n"
         "\n"
         "FORMAT FILE\n"
         "\n"
         "\tDelimiter: <delimiter string, may be multiple characters>\n"
         "\tQuote: <quoting character surrounding fields>\n"
         "\tEscape: <escape character>\n"
         "\tAllow Fewer Columns: <1|0.  Defaults to 0.  Allow less than the total number of columns on a line.>\n"
         "\tColumn List:\n"
         "\t  column1\n"
         "\t  column2\n"
         "\t  Each column must start with whitespace\n"
         "\t  ...\n"
         "\n"
         "\tDelimiter, Quote, and Escape can be literal characters with the exception of whitespace or\n"
         "\thexadecimal ascii in 0x?? format.  Delimiter and column list are required.\n\n"
  );
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


static int
validate_header(void)
{ /* {{{ */
  String *    found;
  PDTColumn * expected;

  String *    separator;
  String *    found_fields;
  String *    expected_fields;
  int         valid;

  /* Initialize */
  found           = null_String;
  expected        = null_PDTColumn;

  found_fields    = null_String;
  expected_fields = null_String;
  separator       = new_String("|", 1);
  valid           = 1;

  void *
  column_join(Array * array, ArrayElement * element, void * target)
  {
    ((String *)target)->m->append((String *)target, ((PDTColumn *)element->data)->column_name);
    if(array->position < (array->length - 1)) {
      ((String *)target)->m->append((String *)target, separator);
    }
    return target;
  }


  /* Validate Number of Fields */
  if(header_fields->length != format->columns->length) {
    found_fields = new_String("", 0);
    found_fields->m->join(found_fields, header_fields, separator);
    valid = 0;
    expected_fields = new_String("", 0);
    format->columns->m->join(format->columns, expected_fields, column_join);
    error("Wrong number of columns in file header\n"
          "   Found: [%s]\n"
          "Expected: [%s]\n", found_fields->string, expected_fields->string);
  }

  /* Validate Field Contents */
  if(valid == 1) {
    header_fields->m->reset_each(header_fields);
    format->columns->m->reset_each(format->columns);
    while(valid == 1
          && format->columns->m->each(format->columns) != null_ArrayElement
          && header_fields->m->each(header_fields) != null_ArrayElement) {
      expected = (PDTColumn *)format->columns->current_element->data;
      found = (String *)header_fields->current_element->data;
      expected->column_name->m->upcase(expected->column_name);
      found->m->upcase(found);
      if(expected->column_name->m->compare(expected->column_name, found) != 0) {
        valid = 0;
      }
    }

    /* Column is match found */
    if(valid == 0) {
      found_fields = new_String("", 0);
      found_fields->m->join(found_fields, header_fields, separator);
      expected_fields = new_String("", 0);
      format->columns->m->join(format->columns, expected_fields, column_join);
      error("Invalid column name in column %i.  Expected %s, but found %s.\n"
            "   Found: [%s]\n"
            "Expected: [%s]\n", format->columns->position + 1, expected->column_name->string, found->string, found_fields->string, expected_fields->string);
    }
  }

  separator->m->free(separator);
  found_fields->m->free(found_fields);
  expected_fields->m->free(expected_fields);

  return valid;
} /* }}} */


static void
header_field(ParseDelimitedText * parser, String * field)
{ /* {{{ */
  header_fields->m->append(header_fields, field->m->dup(field));
} /* }}} */


static void
header_record(ParseDelimitedText * parser, char eol)
{ /* {{{ */
  parser->m->stop(parser);
} /* }}} */


static void
field(ParseDelimitedText * parser, String * field)
{ /* {{{ */
  field_count++;
} /* }}} */


static void
record(ParseDelimitedText * parser, char eol)
{ /* {{{ */
  if(parser->record_count > 0) {
    if(field_count != format->columns->length) {
      error("Wrong field count on row %zu, found %i expected %i\n", parser->record_count + 1, field_count, format->columns->length);
      valid = 0;
      parser->m->stop(parser);
    }
  }

  field_count = 0;
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
