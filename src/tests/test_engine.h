/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libpdt is distributed under the terms of the GNU
 *  Lesser General Public License.
 * 
 *  This file is part of libpdt.
 *
 *  libpdt is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  libpdt is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with libpdt.  If not, see
 *  <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#ifndef _TEST_ENGINE_H
#define _TEST_ENGINE_H_

#include <dependencies.h>

#include <class_String.h>
#include <class_Array.h>
#include <class_ArrayElement.h>
#include <class_ParseDelimitedText.h>

#define PDT_END 0
#define PDT_FLD 1
#define PDT_REC 2
#define PDT_ERR 3

#define DO_TEST(name, options) test_parser("test" #name, options, test ## name ## _data, \
        sizeof(test ## name ## _data) - 1, test ## name ## _results, \
        ",", "\"")

#define DO_TEST_CUSTOM(name, options, d, q, s, t) test_parser("custom" #name, options, custom ## name ## _data, \
        sizeof(custom ## name ## _data) - 1, custom ## name ## _results, d, q)

struct event {
  int event_type; 
  int retval;
  size_t size;  /* Number of bytes in this event or return value from csv_fini */
  char *data;   /* Data for this event */
};

struct event *event_ptr;
int event_idx;
size_t row, col;
char *current_test_name;

int test_failed = 0;

/* Prototypes */
void fail_parser(char *message);
void field_callback(ParseDelimitedText *parser, String *parsed_field);
void record_callback(ParseDelimitedText *parser, char eol);
int  run_test(char *test_name, unsigned char options, char *input, size_t len, struct event expected[], char *delimiter, char *quote);


void
fail_parser(char *message)
{
  fprintf(stderr, "# Parser test %s failed on event %d: %s\n", current_test_name, event_idx, message);
  test_failed = 1;
}


void
field_callback(ParseDelimitedText *parser, String *parsed_field)
{
  /* Make sure we were expecting a column */
  if(event_ptr->event_type != PDT_FLD) {
    parser->stop = 1;
    fail_parser("didn't expect a column");
  }

  /* Check the actual size against the expected size */
  if(event_ptr->size != parsed_field->length) {
    parser->stop = 1;
    fail_parser("actual data length doesn't match expected data length");
  }

  /* Check the actual data against the expected data */
  if(memcmp(event_ptr->data, parsed_field->string, event_ptr->size) != 0) {
    parser->stop = 1;
    fail_parser("actual data doesn't match expected data");
  }

  event_idx++;
  event_ptr++;
  col++;
}


void
record_callback(ParseDelimitedText *parser, char eol)
{
  /* Make sure we were expecting an the end of a row */
  if(event_ptr->event_type != PDT_REC) {
    parser->stop = 1;
    fail_parser("didn't expect end of row");
  }

  /* Check that the row ended with the character we expected */
  if(event_ptr->retval != eol) {
    parser->stop = 1;
    fail_parser("row ended with unexpected character");
  }

  event_idx++;
  event_ptr++;
  col = 1;
  row++;
}


int
run_test(
  char          *test_name,
  unsigned char options,
  char          *input,
  size_t        len,
  struct event  expected[],
  char          *delimiter,
  char          *quote
)
{
  ParseDelimitedText *parser;
  int result = 0;
  size_t retval;
  size_t size;

  for(size = 1; size <= len; size++) {
    size_t bytes_processed = 0;
    current_test_name = test_name;
    parser = new_ParseDelimitedText(options);
    parser->m->set_delimiter(parser, delimiter, strlen(delimiter));
    parser->m->set_quote(parser, quote, strlen(quote));
    parser->field_callback = field_callback;
    parser->record_callback = record_callback;

    row = col = 1;
    event_ptr = &expected[0];
    event_idx = 1;

    do {
      size_t bytes = size < len - bytes_processed ? size : len - bytes_processed;
      retval = parser->m->parse(parser, input + bytes_processed, bytes);
      if(retval != bytes) {
        if(event_ptr->event_type != PDT_ERR) {
          fail_parser("unexpected parse error occured");
        }
      }
      bytes_processed += bytes;
    } while(bytes_processed < len);

    result = parser->m->finish(parser);

    if(result != 0) {
      if(event_ptr->event_type != PDT_ERR) {
        fail_parser("unexpected parse error occured");
      }
      else {
        parser->m->free(parser);
        return 0;
      }
    }

    parser->m->free(parser);

    if(event_ptr->event_type != PDT_END) {
      fail_parser("unexpected end of input");
      return 0;
    }
  }

  return 0;
}

#endif /* not _TEST_ENGINE_H_ */
