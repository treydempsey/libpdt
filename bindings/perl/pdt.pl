#!/opt/local/bin/perl

use strict;
use warnings;

use lib qw(blib/lib blib/arch);

use ExtUtils::Command::MM;
use PDT;

our $number_of_fields = 0;

sub field
{
  my ($parser, $field_data) = @_;

  $number_of_fields++;

  print "delimiter: ", $parser, "\n";

  print "field: >$field_data<\n";
}

sub record
{
  my ($parser, $eol) = @_;
  printf("record: >%02x<\n", ord($eol));
  print "Found $number_of_fields fields\n";
  $number_of_fields = 0;
}


my $data = "a~|~bc~|~def\x0a";

my $parser = ParseDelimitedText::new(0);
print "parser: $parser\n";
$parser->field_callback = \&field;
print "field: ", \&field, "\n";
print "field_callback: ", $parser->field_callback, "\n";
$parser->record_callback = \&record;
print "record: ", \&record, "\n";
print "record_callback: ", $parser->record_callback, "\n";
$parser->delimiter = '~|~';
#print "Delimiter: ", $parser->delimiter, "\n";
print "data: >$data<\n";
print "parser $parser\n";

$parser->parse($data);
$parser->finish();

print "Before \$parser = undef\n";

$parser = undef;

print "After \$parser = undef\n";
