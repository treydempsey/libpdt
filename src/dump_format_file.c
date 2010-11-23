/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libpdt is distributed under the terms of the GNU Lesser General Public
 *  License.
 * 
 *  This file is part of libpdt.
 *
 *  libpdt is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  libpdt is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with libpdt.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "dependencies.h"

#include <class_PDTFormatFile.h>

int main(int argc, char *argv[], char *envp[])
{
  PDTFormat *     f;
  PDTColumn *     pdt_column;
  PDTValidation * pdt_validation;
  String *        argument;

  if(argc != 2) {
    printf("Usage: %s <format file>\n", argv[0]);
    exit(1);
  }

  f = new_PDTFormat();
  f->m->read_file(f, argv[1], strlen(argv[1]));

  printf("Allow Fewer Columns = %s\n", (f->allow_fewer_columns == 0) ? "no" : "yes");
  printf("          Delimiter = (0x%02x) %s\n", (unsigned int)*f->delimiter->string, f->delimiter->string);
  printf("             Escape = (0x%02x) %s\n", (unsigned int)*f->escape->string, f->escape->string);
  printf("              Quote = (0x%02x) %s\n", (unsigned int)*f->quote->string, f->quote->string);
  printf("               Skip = %i\n", f->skip);

  printf("        Column List:\n");
  f->columns->m->reset_each(f->columns);
  while(f->columns->m->each(f->columns) != null_ArrayElement) {
    pdt_column = (PDTColumn *)f->columns->current_element->data;
    printf("                    = %s\n", pdt_column->column_name->string);
    pdt_column->validations->m->reset_each(pdt_column->validations);
    while(pdt_column->validations->m->each(pdt_column->validations) != null_ArrayElement) {
      pdt_validation = (PDTValidation *)pdt_column->validations->current_element->data;
      printf("                      - %s\n", pdt_validation->validation_name->string);
      pdt_validation->arguments->m->reset_each(pdt_validation->arguments);
      while(pdt_validation->arguments->m->each(pdt_validation->arguments) != null_ArrayElement) {
        argument = (String *)pdt_validation->arguments->current_element->data;
        printf("                        + >%s<\n", argument->string);
      }
    }
  }
  
  f->m->free(f);

  return 0;
}
