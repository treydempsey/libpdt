/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libparse_delimited_text is distributed under the terms of the GNU
 *  Lesser General Public License.
 * 
 *  This file is part of libparse_delimited_test.
 *
 *  libparse_delimited_test is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  libparse_delimited_test is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with libparse_delimited_text.  If not, see
 *  <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |1|2|3||
     |4||
     |||       */
  char data[] = "1, 2, 3,\x0a\x0d\x0a  \"4\", \x0d,";
  struct event results[] =
    { {DELIM_COL, 0, 1, "1"},
      {DELIM_COL, 0, 1, "2"},
      {DELIM_COL, 0, 1, "3"},
      {DELIM_COL, 0, 0, ""},
      {DELIM_ROW, '\x0a', 1, NULL},
      {DELIM_COL, 0, 1, "4"},
      {DELIM_COL, 0, 0, ""},
      {DELIM_ROW, '\x0d', 1, NULL},
      {DELIM_COL, 0, 0, ""},
      {DELIM_COL, 0, 0, ""},
      {DELIM_ROW, 0, 0, NULL}, {DELIM_END, 0, 0, NULL} }; 
      
  run_test("test_14-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
