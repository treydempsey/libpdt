package Text::ParseDelimited;

use 5.008009;
use strict;
use warnings;
use Carp;

require Exporter;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use Text::ParseDelimited ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
  PDT_STATIC PDT_STATIC_FINI PDT_REPALL_NL	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('Text::ParseDelimited', $VERSION);

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

Text::ParseDelimited - Perl extension for parsing delimited text.

=head1 SYNOPSIS

  use Text::ParseDelimited;

  my $fields = [];

  sub field
  {
    my ($parser, $field) = @_;
    push(@$fields, $field);
  }

  sub record
  {
    my ($parser, $eol) = @_;
    print "Found: ", join(@$fields, ", "), "\n";
    @$fields = [];
  }

  my $parser = Text::ParseDelimited::new(0);
  $parser->delimiter = "|";
  $parser->field_callback = \&field;
  $parser->record_callback = \&record;

  open(my $f, "<", "input.txt");
  while(read($f, my $data, 100)) {
    $parser->parse($data);
  }
  $parser->finish();
  close($f);

=head1 DESCRIPTION

Text::ParseDelimited is a perl binding for libpdt, a library for parsing
delimited text.  It supports multicharacter delimiters, quoting, and has
improper quote handling.  Although it can be configured to parse comma
separated values, it is designed to be a more generic delmited text parser.

=head2 EXPORT

None by default.

=head1 SEE ALSO

http://github.com/libpdt
http://github.com/libtgcl

=head1 AUTHOR

Trey Dempsey, E<lt>trey.dempsey@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright 2010  Trey Dempsey

libpdt is distributed under the terms of the GNU Lesser General Public
License.

This file is part of libpdt.

libpdt is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option)
any later version.

libpdt is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with libpdt.  If not, see <http://www.gnu.org/licenses/>.


=cut
