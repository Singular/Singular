#!/usr/local/bin/perl
# $Id: pl2doc.pl,v 1.2 1999-07-21 19:53:26 obachman Exp $
###################################################################
#  Computer Algebra System SINGULAR
#
#  pl2doc.pl: utility to generate doc file out of pl for singular library
#
####
$Usage = <<EOT;
Usage:  
$0 [-o out_file -db db_file -no_ex -no_fun] library_perl_file.doc 
Convert library_perl_file.pl to library_perl_file.doc
EOT

###################################################################
# parse command line options
#
while (@ARGV && $ARGV[0] =~ /^-/) 
{
  $_ = shift(@ARGV);
  if (/^-o$/)    { $out_file = shift(@ARGV); next;}
  if (/^-db$/) { $db_file = shift(@ARGV); next;}
  if (/^-no_fun$/)    { $no_fun = 1;next;}
  if (/^-h(elp)?$/)      { print $Usage; exit;}
  die "Error: Unknown option: $_:$Usage\n";
}


###################################################################
# Execute perl file
#
$pl_file = pop(@ARGV);
die "Error: No perl file specified: $Usage\n" unless $pl_file;
die "Error: Can't find perl file $pl_file: $Usage\n" unless -r $pl_file;
require $pl_file;
($lib = $library) =~ s|.*/(.*)\.lib$|$1|;

###################################################################
# print  summary
#
unless ($out_file)
{
  ($out_file = $pl_file) =~ s/(.*)\..*$/$1/;
  $out_file .= "_noFun" if ($no_fun);
  $out_file .= ".doc";
}
open(LDOC, ">$out_file") || die"Error: can't open $out_file for writing: $!\n";
print LDOC "\@c library version: $version\n";
print LDOC "\@c library file: $library\n";

undef @procs; # will be again defined by OutLibInfo
$ref = OutLibInfo(\*LDOC, $info, ! $no_fun);
OutRef(\*LDOC, $ref) if $ref;


###################################################################
# print  summary
#
# print subsections for help of procs
unless ($no_fun)
{
  if ($db_file)
  {
    $db_file = $1 if ($db_file =~ /(.*)\.db$/);
    dbmopen(%CHECKSUMS, $db_file, oct(755)) ||
      die "Error: can't open chksum data base $db_file";
  }
  # print help and example of each function
  for ($i = 0; $i <= $#procs; $i++)
  {
    # print node and section heading
    print LDOC "\n\@c ------------------- " . $procs[$i]." -------------\n";
    print LDOC "\@node " . $procs[$i].",";
    print LDOC " ".$procs[$i+1] if ($i < $#procs);
    print LDOC ",";
    print LDOC " ".$procs[$i-1] if ($i > 0);
    print LDOC ", " . $lib ."_lib\n";
    print LDOC "\@subsection " . $procs[$i] . "\n";
    print LDOC "\@cindex ". $procs[$i] . "\n";
    $CHECKSUMS{$procs[$i]} = $chksum{$procs[$i]} if ($db_file);
    print LDOC "\@c ---content $procs[$i]---\n";
    print LDOC "Procedure from library \@code{$lib.lib} (\@pxref{${lib}_lib}).\n\n";
    if ($help{$procs[$i]} =~ /^\@/)
    {
      print LDOC $help{$procs[$i]};
      $ref = '';
    }
    else
    {
      print LDOC "\@table \@asis\n";
      $table_is_open = 1;
      # print help
      $ref = OutInfo(\*LDOC, $help{$procs[$i]});
      print LDOC "\@end table\n";
    }
    # print example
    if ($ex = &CleanUpExample($lib, $example{$procs[$i]}))
    {
      print LDOC "\@strong{Example:}\n";
      print LDOC "\@smallexample\n\@c example\n";
      print LDOC $ex;
      print LDOC "\n\@c example\n\@end smallexample\n";
    }
    OutRef(\*LDOC, $ref) if $ref;
    print LDOC "\@c ---end content $procs[$i]---\n";
  }
  dbmclose(%CHECKSUMS);
}

# 
# und Tschuess
#
close(LDOC);
exit(0);

###########################################################################
#
# parse and print-out libinfo
#
sub OutLibInfo
{
  my ($FH, $info, $l_fun) = @_;
  print $FH "\@c ---content LibInfo---\n";
    if ($info =~ /^\@/)
  {
    print $FH $info;
    return;
  }
  print $FH "\@table \@asis\n";
  $table_is_open = 1;
  
  my ($ref) = OutInfo($FH, $info, $l_fun);

  print $FH "\@end table\n" if $table_is_open;
  print $FH "\@c ---end content LibInfo---\n";
  $table_is_open = 0;
  return $ref;
}

sub OutInfo
{
  my ($FH, $info, $l_fun) = @_;
  if ($info =~ /^\s*\@/)
  {
    print $FH $info;
    return;
  }
  $info =~ s/^\s*//;
  $info =~ s/\s*$//;
  $info .= "\n";

  my ($item, $text, $line, $ref);
  while ($info =~ m/(.*\n)/g)
  {
    $line = $1;
    if ($1 =~ /^(\w.+?):(.*\n)/)
    {
      $ref .= OutInfoItem($FH, $item, $text, $l_fun) if $item && $text;
      $item = $1;
      $text = $2;
    }
    else
    {
      $text .= $line;
    }
  }
  $ref .= OutInfoItem($FH, $item, $text, $l_fun) if $item && $text;
  return $ref;
}

sub FormatInfoText
{
  my $length = shift;
  $length = 0 unless $length;
  # insert @* infront of all lines whose previous line is shorter than
  # 60 characters
  $_ = ' ' x $length . $_;
  if (/^(.*)\n/)
  {
    $_ .= "\n";
    my $pline;
    my $line;
    my $ptext = $_;
    my $text = '';
    while ($ptext =~ /(.*)\n/g)
    {
      $line = $1;
      # break line if
      $text .= '@*' 
	if ($line =~ /\w/ && $pline =~ /\w/ # line and prev line are not empty
	    && $line !~ /^\s*\@\*/  # line does not start with @*
	    && $pline !~ /\@\*\s*/  # prev line does not end with @*
	    &&
	    ((length($pline) < 60  && # prev line is shorter than 60 chars
	      $pline !~ /\@code{.*?}/ # and does not contain @code, @math
	      && $pline !~ /\@math{.*?}/) 
	     ||
	     $line =~ /^\s*\w*\(.*?\)/ # $line starts with \w*(..)
	     ||
	     $pline =~ /^\s*\w*\(.*?\)[\s;:]*$/)); # prev line is only \w(..)
      $line =~ s/\s*$//;
      $text .= "$line\n";
      $pline = $line;
    }
    $_ = $text;
  }
  s/\t/ /g;
  s/\n +/\n/g;
  s/\s*$//g;
  s/ +/ /g;  # replace double whitespaces by one
  s/(\w+\(.*?\))/\@code{$1}/g;
  s/\@\*\s*/\@\*/g;
  s/(\@[^\*])/\@$1/g; # escape @ signs, except @*
  s/{/\@{/g; # escape {}
  s/}/\@}/g;
  # unprotect @@math@{@}, @code@{@}
  while (s/\@\@math\@{(.*?)\@}/\@math{$1}/g) {} 
  while (s/\@\@code\@{(.*?)\@}/\@code{$1}/g) {}
  # remove @code{} inside @code{} and inside @math{}
  while (s/\@math{([^}]*)\@code{(.*?)}(.*)?}/\@math{$1$2$3}/g) {}
  while (s/\@code{([^}]*)\@code{(.*?)}(.*)?}/\@code{$1$2$3}/g) {}
}

sub OutInfoItem
{
  my ($FH, $item, $text, $l_fun) = @_;

  $item = lc $item;
  $item = ucfirst $item;

  if ($item =~ /see also/i)
  {
    # return references
    return $text;
  }
  elsif ($item =~ m/example/i)
  {
    # forget about example, since it comes explicitely
    return '';
  }
  elsif ($item =~ m/procedure/i)
  {
    if ($l_fun && $table_is_open)
    {
      print $FH "\@end table\n\n";
      $table_is_open = 0;
    }
    $text =~ s/^\s*//;
    $text =~ s/\s*$//;
    $text =~ s/.*$// if ($text=~/parameters.*brackets.*are.*optional.*$/);
    $text .= "\n";
    
    my ($proc, $pargs, $pinfo, $line);
    if ($l_fun)
    {
      print $FH "\@strong{$item:}\n\@menu\n";
    }
    else
    {
      print $FH "\@item \@strong{$item:}\n\@table \@asis\n";
    }
    while ($text =~ /(.*\n)/g)
    {
      $line = $1;
      if ($1 =~ /^\s*(\w+)\((.*?)\)/)
      {
	OutProcInfo($FH, $proc, $procargs, $pinfo, $l_fun) if $proc && $pinfo;
	$proc = $1;
	$procargs = $2;
	$pinfo = $';
      }
      else
      {
	$pinfo .= $line; 
      }
    }
    OutProcInfo($FH, $proc, $procargs, $pinfo, $l_fun) if $proc && $pinfo;
    print $FH ($l_fun ? "\@end menu\n" : "\@end table\n");
    return '';
  }

  if (! $table_is_open)
  {
    print $FH "\@table \@asis\n";
    $table_is_open = 1;
  }
  print $FH '@item @strong{'. "$item:}\n";
  # prepare text:
  local $_ = $text;
  if (($item =~ m/^library/i)  && m/\s*(\w*)\.lib/)
  {
    print $FH "$1.lib\n";
    $text = $';
    if ($text =~ /\w/)
    {
      print $FH '@item @strong{Purpose:'."}\n";
      print $FH lc $text;
    }
  }
  else
  {
    # just print the text
    FormatInfoText(length($item) + 1);
    print $FH "$_\n";
  }
  return '';
}

sub OutProcInfo
{
  my($FH, $proc, $procargs, $pinfo, $l_fun) = @_;
  local $_ = $pinfo;
  s/^[;\s]*//;
  s/\n/ /g;
  FormatInfoText();
  
  if ($l_fun)
  {
    print $FH "* ${proc}:: $_\n";
    push @procs, $proc;
  }
  else
  {
    print $FH "\@item \@code{$proc($procargs)}  ";
    print $FH "\n\@cindex $proc\n$_\n";
  }
}

sub OutRef
{
  my ($FH, $refs) = @_;
  $refs =~ s/^\s*//;
  $refs =~ s/\s*$//;
  $refs =~ s/\n/,/g;
  my @refs = split (/[,;\.]+/, $refs);
  my $ref;

  print $FH "\@c ref\nSee also:\n";
  $ref = shift @refs;
  print $FH "\@ref{$ref}";
  for $ref (@refs)
  {
    $ref =~ s/^\s*//;
    $ref =~ s/\s*$//;
    print $FH ", \@ref{$ref}"  if ($ref =~ /\w/);
  }
  print $FH "\n\@c ref\n";
}

sub CleanUpExample
{
  local($lib, $example) = @_;
  
  # find portion in {}
  $example =~ s/^[^{]*{(.*)}[^}]*$/$1/s;

  if ($example =~ /EXAMPLE: \(not executed\)/)
  {
    # erase first three lines
    $example =~ s/^.*\n.*\n.*\n/\n/;
    # erase enclosing " " in every line
    $example =~ s/\n\s*"/\n/g;
    $example =~  s/";\n/\n/g;
  }
  # erase EXAMPLE, echo and pause statements
  $example =~ s/"EXAMPLE.*"[^;]*;//g;
  $example .= "\n";
  my ($mexample, $line);
  while ($example =~ m/(.*)\n/g)
  {
    $line = $1;
    $line =~ s|echo[^;]*;||g if $line !~ m|(.*)//(.*)echo[^;]*;|;
    $line =~ s|pause\(.*?\)[^;]*;||g if $line !~ m|(.*)//(.*)pause\(.*?\)[^;]*;|;
    $mexample .= "$line\n";
  }
  $example = $mexample;
  
  # prepend LIB command
  $example = "LIB \"$lib.lib\";\n".$example 
    if ($example && $lib ne "standard");
  # erase empty lines
  $example =~ s/^\s*\n//g;
  # erase spaces from beginning of lines
  $example =~ s/\n\s*/\n/g;
  $example =~ s/\s*$//g;
  return $example;
}

