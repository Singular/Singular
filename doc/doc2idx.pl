#!/usr/local/bin/perl
###################################################################
#  Computer Algebra System SINGULAR
#
#  doc2idx.pl: utility to index file for singular manual
#
#  Prints to stdout lines of the following form
#
#  $indexentry\t$nodename\t$url\t$chksum
#
#  where $indexentry: entries of index (all lower case)
#        $nodename  : name of node where index entry occurred
#        $url       : url of node
#        $chksum    : checksum of help text, if library proc node
# lines are sorted alphabetically w.r.t. $indexentry

####
$Usage = <<EOT;
Usage:
$0 singular-index-file html-index-file chksum-db
Outputs sorted lines of the form $indexentry\t$nodename\t$url\t$chksum
EOT

###################################################################
# parse command line options
#
die "Need 3 arguments:\n $Usage" unless (scalar(@ARGV) == 3);

# open files
open (HLP, "<$ARGV[0]") || die "Can't open $ARGV[0]: $!\n";
open (URL, "<$ARGV[1]") || die "Can't open $ARGV[1]: $!\n";
$db_file = $ARGV[2];
unless ($return = do $db_file)
{
  die "couldn't parse $db_file: $@" if $@;
  die "couldn't do $db_file: $!"    unless defined $return;
  die "couldn't run $db_file"       unless $return;
}

# fill hashes
# first: HLP
while (<HLP>)
{
  if (/Node: Index/)
  {
    while (<HLP>)
    {
      last if /\* Menu:/;
    }
    <HLP>;
    while (<HLP>)
    {
      last if /^\s*$/;
      if (/^\* (.*?): (.*)\.$/)
      {
	$entry = $1;
	$node = $2;
      }
      $entry =~ s/\s*(.*) <\d+>\s*$/$1/;
      $node =~ s/^\s*(.*)\s*$/$1/;
      $index->{$entry}->{Node} = $node unless $entry eq $prev;
      $prev = $entry;
    }
    last;
  }
}
close(HLP);

while (<URL>)
{
  ($entry, $url) = split(/\t/);
  $url =~ s/^\s*(.*)\s*$/$1/;
  $index->{$entry}->{Url} = $url;
}
close(URL);

for $entry (sort keys %$index)
{
  print "$entry\t$index->{$entry}->{Node}\t$index->{$entry}->{Url}\t$CHECKSUMS{$entry}\n";
}


