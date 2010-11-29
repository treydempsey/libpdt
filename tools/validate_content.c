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


typedef struct ValidateContentCTX {
  Array *               record;
  Array *               record_errors;
  ParseDelimitedText *  parser;
  PDTFormat *           format;
  PDTValidator *        validator;
  String *              bad_buffer;
  String *              bad_column_header;
  String *              bad_file;
  String *              format_file;
  String *              input_file;
  String *              output_buffer;
  String *              output_file;

  char                  input_file_buffer[PDT_BLK_SIZE];

  int                   input_file_fd;
  int                   output_file_fd;
  int                   bad_file_fd;
  int                   field_count;
  int                   input_records;
  int                   output_records;
  int                   bad_records;
  int                   valid;
  int                   record_error_count;

  size_t                bytes_read;
} ValidateContentCTX;
ValidateContentCTX * ctx;


/* Prototypes */
static void             set_params(ValidateContentCTX * ctx, int argc, char * argv[]);
static void             open_input_file(ValidateContentCTX * ctx);
static void             close_input_file(ValidateContentCTX * ctx);
static void             open_output_file(ValidateContentCTX * ctx);
static void             write_output_file(ValidateContentCTX * ctx);
static void             close_output_file(ValidateContentCTX * ctx);
static void             open_bad_file(ValidateContentCTX * ctx);
static void             write_bad_file(ValidateContentCTX * ctx);
static void             close_bad_file(ValidateContentCTX * ctx);
static void             summary(ValidateContentCTX * ctx);
static void             validate_record(ValidateContentCTX * ctx, ParseDelimitedText * parser);
static void             append_record_errors(ValidateContentCTX * ctx, String * column_name, String * validation_name);
void                    write_output_buffer(ValidateContentCTX * ctx, String * data);
void                    write_output_buffer_cstr(ValidateContentCTX * ctx, char * data, size_t length);
void                    write_bad_buffer(ValidateContentCTX * ctx, String * data);
void                    write_bad_buffer_cstr(ValidateContentCTX * ctx, char * data, size_t length);

static void             usage(char * argv0);
static void             fieldcb(ParseDelimitedText * parser, String * field);
static void             recordcb(ParseDelimitedText * parser, char eol);
static void             error(const char * format, ...);


int
main(int argc, char * argv[], char * envp[])
{
  /* Initialize our context */
  ctx = malloc(sizeof(ValidateContentCTX));
  if(ctx == NULL) {
    perror("ERROR: malloc() ValidateContentCTX");
    exit(-1);
  }
  memset(ctx, 0, sizeof(ValidateContentCTX));

  /* Parse command line parameters */
  set_params(ctx, argc, argv);
  
  /* Load the format file */
  ctx->format = new_PDTFormat();
  ctx->format->m->read_file(ctx->format, ctx->format_file->string, ctx->format_file->length);

  /* Create a validator instance */
  ctx->validator = new_PDTValidator();

  /* Open the files */
  open_input_file(ctx);
  open_output_file(ctx);
  open_bad_file(ctx);

  /* Set the parser's options */
  ctx->parser = new_ParseDelimitedText(0);
  ctx->parser->m->apply_format(ctx->parser, ctx->format);
  ctx->parser->m->set_block_size(ctx->parser, PDT_BLK_SIZE);
  ctx->parser->m->set_delimiter(ctx->parser, "\x1f", 1);
  ctx->parser->m->set_field_callback(ctx->parser, fieldcb);
  ctx->parser->m->set_record_callback(ctx->parser, recordcb);
  ctx->parser->user_data = ctx;

  /* Initialize Context Variables */
  ctx->record = new_Array();
  ctx->record->auto_free = null_String->m->Array_free;
  ctx->record_errors = new_Array();
  ctx->record_errors->auto_free = null_String->m->Array_free;
  ctx->format->columns->m->reset_each(ctx->format->columns);
  while(ctx->format->columns->m->each(ctx->format->columns) != null_ArrayElement) {
    ctx->record->m->append(ctx->record, new_String("", 0));
  }
  ctx->field_count = 0;
  ctx->record_error_count = 0;
  ctx->input_records = 0;
  ctx->output_buffer = new_String("", 0);
  ctx->output_buffer->m->extend(ctx->output_buffer, 8191);
  ctx->output_records = 0;
  ctx->bad_buffer = new_String("", 0);
  ctx->bad_buffer->m->extend(ctx->bad_buffer, 8191);
  ctx->bad_records = 0;
  ctx->bad_column_header = new_String("ERROR_MESSAGES", 14);
  ctx->valid = 1;

  /* Validate the file */
  printf("Reading %s\n", basename(ctx->input_file->string));
  while(ctx->parser->stop == 0
        && (ctx->bytes_read = read(ctx->input_file_fd, ctx->input_file_buffer, PDT_BLK_SIZE)) != 0) {
    ctx->parser->m->parse(ctx->parser, ctx->input_file_buffer, ctx->bytes_read);
  }
  ctx->parser->m->finish(ctx->parser);
  if(ctx->output_file->length != 0) {
    write_output_file(ctx);
  }
  if(ctx->bad_file->length != 0) {
    write_bad_file(ctx);
  }
  summary(ctx);

  /* Cleanup Files */
  close_input_file(ctx);
  close_output_file(ctx);
  close_bad_file(ctx);

  /* Cleanup Variables */
  ctx->record->m->free(ctx->record);
  ctx->parser->m->free(ctx->parser);
  ctx->format->m->free(ctx->format);
  ctx->validator->m->free(ctx->validator);
  ctx->bad_buffer->m->free(ctx->bad_buffer);
  ctx->bad_file->m->free(ctx->bad_file);
  ctx->bad_column_header->m->free(ctx->bad_column_header);
  ctx->format_file->m->free(ctx->format_file);
  ctx->input_file->m->free(ctx->input_file);
  ctx->output_buffer->m->free(ctx->output_buffer);
  ctx->output_file->m->free(ctx->output_file);

  return (ctx->valid) ? SHELL_TRUE : SHELL_FALSE;
}


void
set_params(ValidateContentCTX * ctx, int argc, char * argv[])
{ /* {{{ */
  struct stat file_stat;

  if(argc != 5) {
    usage(argv[0]);
    exit(SHELL_FALSE);
  }

  
  /* Format File */
  if(stat(argv[1], &file_stat) == -1) {
    perror("ERROR: stat() error for format file");
    exit(SHELL_FALSE);
  }
  if(! S_ISREG(file_stat.st_mode)) {
    error("Format file is not a regular file.\n");
    exit(SHELL_FALSE);
  }
  ctx->format_file = new_String(argv[1], strlen(argv[1]));

  /* Input File */
  if(stat(argv[2], &file_stat) == -1) {
    perror("ERROR: stat() error for input file");
    exit(SHELL_FALSE);
  }
  if(! S_ISREG(file_stat.st_mode)) {
    error("Input file is not a regular file.\n");
    exit(SHELL_FALSE);
  }
  ctx->input_file = new_String(argv[2], strlen(argv[2]));

  /* Output File */
  if(stat(argv[3], &file_stat) == -1) {
    if(errno != ENOENT) {
      perror("ERROR: stat() error for output file");
      exit(SHELL_FALSE);
    }
  }
  else if(! S_ISREG(file_stat.st_mode)) {
    error("Output file is not a regular file.\n");
    exit(SHELL_FALSE);
  }
  ctx->output_file = new_String(argv[3], strlen(argv[3]));

  /* Bad File */
  if(stat(argv[4], &file_stat) == -1) {
    if(errno != ENOENT) {
      perror("ERROR: stat() error for bad file");
      exit(SHELL_FALSE);
    }
  }
  else if(! S_ISREG(file_stat.st_mode)) {
    error("Bad file is not a regular file.\n");
    exit(SHELL_FALSE);
  }
  ctx->bad_file = new_String(argv[4], strlen(argv[4]));

  /* Ensure input_file != output_file
           output_file != bad_file
              bad_file != input_file */
  if(   ctx->input_file->m->compare(ctx->input_file, ctx->output_file) == 0
     || ctx->output_file->m->compare(ctx->output_file, ctx->bad_file) == 0
     || ctx->bad_file->m->compare(ctx->bad_file, ctx->input_file) == 0) {
    error("Input, output, and bad file cannot be the same\n");
    exit(SHELL_FALSE);
  }
} /* }}} */


static void
usage(char * argv0)
{ /* {{{ */
  printf("\nUsage: %s <format file> <input file> <output file> <bad file>\n", basename(argv0));
  printf("\n"
         "DESCRIPTION\n"
         "\n"
         "\tValidates each row of data file using format file.  Exits with error code 0 if the file is valid,\n"
         "\tand error code 2 if the file is not valid.\n"
         "\n"
         "VALIDATIONS\n"
         "\n"
         "\tCheck each row against it's defined validations.\n"
         "\n"
         "FORMAT FILE\n"
         "\n"
         "\tDelimiter: <delimiter string, may be multiple characters>\n"
         "\tQuote: <quoting character surrounding fields>\n"
         "\tEscape: <escape character>\n"
         "\tAllow Fewer Columns: <1|0.  Defaults to 0.  Allow less than the total number of columns on a line.>\n"
         "\tColumn List:\n"
         "\t  column1\n"
         "\t    - validation1\n"
         "\t    - validation2(arguments)\n"
         "\t  column2\n"
         "\t    - not_null\n"
         "\t    - length(10)\n"
         "\t  ...\n"
         "\n"
         "\tEach column must start with whitespace\n"
         "\n"
         "\tDelimiter, Quote, and Escape can be literal characters with the exception of whitespace or\n"
         "\thexadecimal ascii in 0x?? format.  Delimiter and column list are required.\n\n"
  );
} /* }}} */


/* File Handling */

static void
open_input_file(ValidateContentCTX * ctx)
{ /* {{{ */
  struct stat file_stat;

  if(stat(ctx->input_file->string, &file_stat) == -1) {
    perror("ERROR: stat() error for input file");
    exit(1);
  }

  if(! S_ISREG(file_stat.st_mode)) {
    error("Input file is not a regular file.\n");
    exit(1);
  }

  if((ctx->input_file_fd = open(ctx->input_file->string, O_RDONLY)) == -1) {
    perror("ERROR: Error opening input file");
    exit(1);
  }
} /* }}} */


static void
close_input_file(ValidateContentCTX * ctx)
{ /* {{{ */
  close(ctx->input_file_fd);
} /* }}} */


static void
open_output_file(ValidateContentCTX * ctx)
{ /* {{{ */
  if((ctx->output_file_fd = open(ctx->output_file->string, O_WRONLY | O_CREAT, 0644)) == -1) {
    perror("ERROR: opening output file");
    exit(SHELL_FALSE);
  }
} /* }}} */


static void
write_output_file(ValidateContentCTX * ctx)
{ /* {{{ */
  write(ctx->output_file_fd, ctx->output_buffer->string, ctx->output_buffer->length);
  ctx->output_buffer->m->truncate(ctx->output_buffer);
} /* }}} */


static void
close_output_file(ValidateContentCTX * ctx)
{ /* {{{ */
  close(ctx->output_file_fd);
} /* }}} */


static void
open_bad_file(ValidateContentCTX * ctx)
{ /* {{{ */
  if((ctx->bad_file_fd = open(ctx->bad_file->string, O_WRONLY | O_CREAT, 0644)) == -1) {
    perror("ERROR: opening bad file");
    exit(SHELL_FALSE);
  }
} /* }}} */


static void
write_bad_file(ValidateContentCTX * ctx)
{ /* {{{ */
  write(ctx->bad_file_fd, ctx->bad_buffer->string, ctx->bad_buffer->length);
  ctx->bad_buffer->m->truncate(ctx->bad_buffer);
} /* }}} */


static void
close_bad_file(ValidateContentCTX * ctx)
{ /* {{{ */
  close(ctx->bad_file_fd);
} /* }}} */


static void
validate_record(ValidateContentCTX * ctx, ParseDelimitedText * parser)
{ /* {{{ */
  PDTColumn *     column_ref;
  PDTValidation * validation_ref;

  ctx->format->columns->m->reset_each(ctx->format->columns);
  while(ctx->format->columns->m->each(ctx->format->columns) != null_ArrayElement) {
    column_ref = (PDTColumn *)ctx->format->columns->current_element->data;
    //fprintf(stderr, "Column: %s\n", column_ref->column_name->string);
    column_ref->validations->m->reset_each(column_ref->validations);
    while(column_ref->validations->m->each(column_ref->validations) != null_ArrayElement) {
      validation_ref = (PDTValidation *)column_ref->validations->current_element->data;
      //fprintf(stderr, "Validation: %s: %s\n", validation_ref->validation_name->string, ((String *)ctx->record->elements[ctx->format->columns->position]->data)->string);
      if(ctx->validator->m->dispatch(ctx->validator, validation_ref, (String *)ctx->record->elements[ctx->format->columns->position]->data) == 0) {
        error("Record: %i, Failed validation: %s, %s\n", parser->record_count + 1, validation_ref->validation_name->string, ((String *)ctx->record->elements[ctx->format->columns->position]->data)->string);
        ctx->valid = 0;
        ctx->record_error_count++;
        append_record_errors(ctx, column_ref->column_name, validation_ref->validation_name);
      }
    }
  }
} /* }}} */


static void
append_record_errors(ValidateContentCTX * ctx, String * column_name, String * validation_name)
{ /* {{{ */
  String * error;

  error = new_String(column_name->string, column_name->length);
  error->m->append_cstr(error, " failed ", 8);
  error->m->append_cstr(error, validation_name->string, validation_name->length);
  ctx->record_errors->m->append(ctx->record_errors, error);
} /* }}} */


inline void
write_output_buffer(ValidateContentCTX * ctx, String * data)
{ /* {{{ */
  if((ctx->output_buffer->length + data->length + 2) >= ctx->output_buffer->size) {
    write_output_file(ctx);
  }
  ctx->output_buffer->m->append(ctx->output_buffer, data);
} /* }}} */


inline void
write_output_buffer_cstr(ValidateContentCTX * ctx, char * data, size_t length)
{ /* {{{ */
  if((ctx->output_buffer->length + length + 2) >= ctx->output_buffer->size) {
    write_output_file(ctx);
  }
  ctx->output_buffer->m->append_cstr(ctx->output_buffer, data, length);
} /* }}} */


inline void
write_bad_buffer(ValidateContentCTX * ctx, String * data)
{ /* {{{ */
  if((ctx->bad_buffer->length + data->length + 2) >= ctx->bad_buffer->size) {
    write_bad_file(ctx);
  }
  ctx->bad_buffer->m->append(ctx->bad_buffer, data);
} /* }}} */


inline void
write_bad_buffer_cstr(ValidateContentCTX * ctx, char * data, size_t length)
{ /* {{{ */
  if((ctx->bad_buffer->length + length + 2) >= ctx->bad_buffer->size) {
    write_bad_file(ctx);
  }
  ctx->bad_buffer->m->append_cstr(ctx->bad_buffer, data, length);
} /* }}} */


static void
summary(ValidateContentCTX * ctx)
{ /* {{{ */
  printf("Writing %s\n", basename(ctx->output_file->string));
  printf("Writing %s\n", basename(ctx->bad_file->string));
  printf("Input: %i record%s\n", ctx->input_records, (ctx->input_records == 1) ? "" : "s");
  printf("Ouput: %i record%s\n", ctx->output_records, (ctx->output_records == 1) ? "" : "s");
  printf("  Bad: %i record%s\n", ctx->bad_records, (ctx->bad_records == 1) ? "" : "s");
} /* }}} */


/* Callbacks */

static void
fieldcb(ParseDelimitedText * parser, String * field)
{ /* {{{ */
  ValidateContentCTX * ctx;

  ctx = (ValidateContentCTX *)parser->user_data;
  if(ctx->field_count <= (ctx->record->length - 1)) {
    ((String *)ctx->record->elements[ctx->field_count]->data)->m->truncate((String *)ctx->record->elements[ctx->field_count]->data);
    ((String *)ctx->record->elements[ctx->field_count]->data)->m->append((String *)ctx->record->elements[ctx->field_count]->data, field);
  }
  ctx->field_count++;
} /* }}} */


static void
recordcb(ParseDelimitedText * parser, char eol)
{ /* {{{ */
  ValidateContentCTX *  ctx;

  ctx = (ValidateContentCTX *)parser->user_data;

  if(ctx->field_count != ctx->record->length) {
    error("Wrong field count on row %zu, found %i expected %i\n", parser->record_count + 1, ctx->field_count, ctx->record->length);
    parser->m->stop(parser);
  }
  else {
    /* Write Header */
    if(parser->record_count == 0) {
      /* Output File Header {{{ */
      ctx->record->m->reset_each(ctx->record);
      while(ctx->record->m->each(ctx->record) != null_ArrayElement) {
        write_output_buffer(ctx, (String *)ctx->record->current_element->data);
        if(ctx->record->position < (ctx->record->length - 1)) {
          write_output_buffer_cstr(ctx, "\x1f", 1);
        }
      }
      write_output_buffer_cstr(ctx, "\x0a", 1);
      /* }}} */

      /* Bad File Header {{{ */
      ctx->record->m->reset_each(ctx->record);
      while(ctx->record->m->each(ctx->record) != null_ArrayElement) {
        write_bad_buffer(ctx, (String *)ctx->record->current_element->data);
        write_bad_buffer_cstr(ctx, "\x1f", 1);
      }
      write_bad_buffer(ctx, ctx->bad_column_header);
      write_bad_buffer_cstr(ctx, "\x0a", 1);
      /* }}} */
    }
    /* Validate each field in record */
    else {
      ctx->input_records++;

      validate_record(ctx, parser);

      if(ctx->record_error_count == 0) {
        /* Write good record to output file {{{ */
        ctx->output_records++;
        ctx->record->m->reset_each(ctx->record);
        while(ctx->record->m->each(ctx->record) != null_ArrayElement) {
          write_output_buffer(ctx, (String *)ctx->record->current_element->data);
          if(ctx->record->position < (ctx->record->length - 1)) {
            write_output_buffer_cstr(ctx, "\x1f", 1);
          }
        }
        write_output_buffer_cstr(ctx, "\x0a", 1);
        /* }}} */
      }
      else {
        /* Write bad record to bad file {{{ */
        ctx->bad_records++;
        ctx->record->m->reset_each(ctx->record);
        while(ctx->record->m->each(ctx->record) != null_ArrayElement) {
          write_bad_buffer(ctx, (String *)ctx->record->current_element->data);
          write_bad_buffer_cstr(ctx, "\x1f", 1);
        }

        /* Write Errors */
        ctx->record_errors->m->reset_each(ctx->record_errors);
        while(ctx->record_errors->m->each(ctx->record_errors) != null_ArrayElement) {
          write_bad_buffer(ctx, (String *)ctx->record_errors->current_element->data);
          if(ctx->record_errors->position < (ctx->record_errors->length - 1)) {
            write_bad_buffer_cstr(ctx, ",", 1);
          }
        }
        write_bad_buffer_cstr(ctx, "\x0a", 1);

        /* Reset the record_errors array */
        ctx->record_errors->m->free(ctx->record_errors);
        ctx->record_errors = new_Array();
        ctx->record_errors->auto_free = null_String->m->Array_free;
        ctx->record_error_count = 0;
        /* }}} */
      }
    }
    ctx->field_count = 0;
  }
} /* }}} */


/* Helpers */

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
  fflush(stderr);

  free(err_fmt);

  va_end(args);
} /* }}} */
