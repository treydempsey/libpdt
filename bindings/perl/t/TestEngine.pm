package TestEngine;

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT = qw(PDT_FLD PDT_REC PDT_END PDT_ERR);

use constant PDT_FLD => 0;
use constant PDT_REC => 1;
use constant PDT_END => 2;
use constant PDT_ERR => 3;

our $events = undef;
our $event_ptr = undef;
our $event_idx = 0;
our $col = 0;
our $current_test_name;
our $test_ok = 1;

# $events
# -------
# Given: "ab|cde|f\x0a"
# 
# $events = [
#             {'event_type' => PTD_FLD, 'size' => 2, 'data' => 'ab'},
#             {'event_type' => PTD_FLD, 'size' => 3, 'data' => 'cde'},
#             {'event_type' => PTD_FLD, 'size' => 1, 'data' => 'f'},
#             {'event_type' => PTD_REC, 'eol' => "\x0a"},
#             {'event_type' => PTD_END}
# ];

sub next_event_ptr
{
  $event_idx++;
  $event_ptr = $events->[$event_idx];
}


sub fail_parser
{
  my ($message) = @_;
  printf(STDERR "# Parser test %s failed on event %d: %s\n", $current_test_name, $event_idx + 1, $message);
  $test_ok = 0;
}


sub field
{
  my ($parser, $parsed_field) = @_;

  # Make sure we were expecting a column
  if($event_ptr->{'event_type'} != PDT_FLD) {
    $parser->stop();
    fail_parser("didn't expect a column");
  }

  # Check the actual size against the expected size
  if($event_ptr->{'size'} != length($parsed_field)) {
    $parser->stop();
    fail_parser("actual data length doesn't match expected data length");
  }

  # Check the actual data against the expected data
  if($event_ptr->{'data'} ne $parsed_field) {
    $parser->stop();
    print("Found: >$parsed_field<\n");
    fail_parser("actual data doesn't match expected data");
  }

  $col++;
  next_event_ptr();
}


sub record
{
  my ($parser, $eol) = @_;

  # Make sure we were expecting an the end of a row
  if($event_ptr->{'event_type'} != PDT_REC) {
    $parser->stop();
    fail_parser("didn't expect end of row");
  }

  # Check that the row ended with the character we expected
  if($event_ptr->{'eol'} ne $eol) {
    $parser->stop();
    fail_parser("row ended with unexpected character");
  }

  $col = 1;
  $row++;
  next_event_ptr();
}


sub run_test
{
  my ($test_name, $options, $input, $len, $expected, $delimiter, $quote) = @_;

  my $parser;
  my $result = 0;
  my $eol;
  my $bytes;
  my $size;
  my $bytes_processed = 0;
  my $data_slice;


  for($size = 1; $size <= $len; $size++) {
    $bytes_processed = 0;
    $current_test_name = $test_name;
    $parser = ParseDelimitedText::new($options);
    $parser->delimiter = $delimiter;
    $parser->quote = $quote;
    $parser->field_callback = \&field;
    $parser->record_callback = \&record;

    $row = $col = 1;
    $events = $expected;
    $event_ptr = $events->[0];
    $event_idx = 0;

    do {
      $bytes = ($size < ($len - $bytes_processed)) ? $size : $len - $bytes_processed;
      $input_slice = substr($input, $bytes_processed, $bytes);
      $eol = $parser->parse($input_slice);
      if($eol != $bytes) {
        if($event_ptr->{'event_type'} != PDT_ERR) {
          fail_parser("unexpected parse error occured");
        }
      }
      $bytes_processed += $bytes;
    } while($bytes_processed < $len);

    $result = $parser->finish();
    $parser = undef;

    if($result != 0) {
      if($event_ptr->{'event_type'} != PDT_ERR) {
        fail_parser("unexpected parse error occured");
      }
      else {
        return 1;
      }
    }

    if($event_ptr->{'event_type'} != PDT_END) {
      fail_parser("unexpected end of input");
    }
  }

  return $test_ok;
}

1;
