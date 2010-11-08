#
#  Copyright 2010  Trey Dempsey
#
#  libpdt is distributed under the terms of the GNU Lesser General Public License.
# 
#  This file is part of libpdt.
#
#  libpdt is free software: you can redistribute it and/or modify it under the
#  terms of the GNU Lesser General Public License as published by the Free
#  Software Foundation, either version 3 of the License, or (at your option)
#  any later version.
#
#  libpdt is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
#  more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with libpdt.  If not, see <http://www.gnu.org/licenses/>.
##################################################################################

use strict;
use warnings;
use lib 't';
use Test::More tests => 1;
use PDT;
use TestEngine;

# |"a,b"|| "" |"" | ""|""|
my $data = "\"\"\"a,b\"\"\",,\" \"\"\"\" \",\"\"\"\"\" \",\" \"\"\"\"\",\"\"\"\"\"\"";
my $results = [
  {'event_type' => PDT_FLD, 'size' => 5, 'data' => "\"a,b\""},
  {'event_type' => PDT_FLD, 'size' => 0, 'data' => ""},
  {'event_type' => PDT_FLD, 'size' => 4, 'data' => " \"\" "},
  {'event_type' => PDT_FLD, 'size' => 3, 'data' => "\"\" "},
  {'event_type' => PDT_FLD, 'size' => 3, 'data' => " \"\""},
  {'event_type' => PDT_FLD, 'size' => 2, 'data' => "\"\""},
  {'event_type' => PDT_REC, 'eol' => "\0"},
  {'event_type' => PDT_END}
];

ok(TestEngine::run_test("test_05-1", 0, $data, length($data), $results, ",", "\""), "parse >$data<");
