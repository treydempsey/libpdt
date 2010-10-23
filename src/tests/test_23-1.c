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
  /* |"aaa"|"b"|"c"| */
  char data[] = "aaa|_b    |_c\x0a";
  struct event results[] = {
    {PDT_FLD, 0, 3, "aaa"},
    {PDT_FLD, 0, 1, "b"},
    {PDT_FLD, 0, 1, "c"},
    {PDT_REC, '\x0a', 1, NULL},
    {PDT_END, 0, 0, NULL}
  };

  run_test("test_23-1", 0, data, sizeof(data) - 1, results, "|_", "\"");

  return test_failed;
}
