#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "class_ParseDelimitedText.h"
#include "class_String.h"
#include "class_Array.h"
#include "class_ArrayElement.h"

static Array *record;

void field_callback(ParseDelimitedText *parser, String *parsed_field)
{
  String *field;
  ArrayElement *element;

  field = parsed_field->m->dup(parsed_field);
  element = new_ArrayElement(field);
  record->m->append(record, element);

  printf("\ncallback field: >>%c<<\n", *parsed_field->string);

}

void record_callback(ParseDelimitedText *parser, char eol)
{
  String *field;

  printf("callback record: >>%02x<<\n", eol);
  printf("record has %i fields.\n", record->length);

  printf("[");
  record->m->reset_each(record);
  while(record->m->each(record) != null_ArrayElement) {
    printf("%s", ((String *)record->current_element->data)->string);
    if(record->current_index + 1 < record->length) {
      printf(", ");
    }
  }
  printf("]\n");

  record->m->reset_each(record);
  while(record->m->each(record) != null_ArrayElement) {
    field = (String *)record->current_element->data;
    field->m->free(field);
  }
  record = record->m->free(record);
  record = new_Array();
}

int main(int argc, char *argv[], char *envp[])
{
  ParseDelimitedText *parser;
  char test_data[] = " 1,2 ,  3         ,4,5\x0d\x0a";

  class_String();
  class_Array();
  class_ArrayElement();
  class_ParseDelimitedText();

  record = new_Array();

  parser = new_ParseDelimitedText(0);
  parser->m->set_delimiter(parser, ",");
  parser->m->set_quote(parser, "\"");
  parser->field_callback = field_callback;
  parser->record_callback = record_callback;

  parser->m->parse(parser, test_data, strlen(test_data));
  parser->m->finish(parser);

  parser->m->free(parser);

  return 0;
}
