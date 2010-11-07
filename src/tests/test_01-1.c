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

#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |1|2|3|4|5| */
  char data[] = " 1,2 ,  3         ,4,5\x0d\x0a";
  struct event results[] = {
    {PDT_FLD, 0, 1, "1"},
    {PDT_FLD, 0, 1, "2"},
    {PDT_FLD, 0, 1, "3"},
    {PDT_FLD, 0, 1, "4"},
    {PDT_FLD, 0, 1, "5"},
    {PDT_REC, '\x0d', 1, NULL},
    {PDT_END, 0, 0, NULL}
  };

  run_test("test_01-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
