#!/usr/local/bin/perl
# $Id$
###################################################################
#  Computer Algebra System SINGULAR
#
#  pl2doc.pl: utility to generate doc file out of pl for singular library
#
####
$Usage = <<EOT;
Usage:  
$0 [-o out_file -db db_file -no_ex -no_fun -doc] library_perl_file.doc 
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
  if (/^-tag$/) { $tag = $tag . " tag:" . shift(@ARGV); next;}
  if (/^-no_fun$/)    { $no_fun = 1;next;}
  if (/^-doc$/)       { $doc = 1; next;}
  if (/^-h(elp)?$/)   { print $Usage; exit;}
  
  die "Error: Unknown option: $_:$Usage\n";
}


###################################################################
# Execute perl file
#
$pl_file = pop(@ARGV);
die "Error: No perl file specified: $Usage\n" unless $pl_file;
die "Error: Can't find perl file $pl_file: $Usage\n" unless -r $pl_file;
require $pl_file;
$lib = $library;
$lib =~ s|.*/(.*)$|$1|;
$lib =~ s|(.*)\.lib$|$1|;
$lib =~ s|(.*)\.so$|$1|;
if ($library =~ /\.so$/) { $so_module="so";} else { $so_module="lib"; }

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
print_doc_header(\*LDOC) if $doc;
print LDOC "\@c ---content LibInfo---\n";
print LDOC "\@c library version: $version\n";
print LDOC "\@c library file: $library\n";
print LDOC "\@cindex $lib.$so_module\n";
print LDOC "\@cindex ${lib}_$so_module\n";
undef @procs; # will be again defined by OutLibInfo
$parsing = "info-string of lib $lib:";
$ref = OutLibInfo(\*LDOC, $info, ! $no_fun);
OutRef(\*LDOC, $ref) if $ref;
print LDOC "\@c ---end content LibInfo---\n";

###################################################################
# print  summary
#
# print subsubsections for help of procs
unless ($no_fun)
{
  if ($db_file && -e $db_file && ! -z $db_file)
  {
    my $return;
    unless ($return = do $db_file)
    {
      die "couldn't parse $db_file: $@" if $@;
      die "couldn't do $db_file: $!"    unless defined $return;
      die "couldn't run $db_file"       unless $return;
    } 
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
    print LDOC "\@subsubsection " . $procs[$i] . "\n";
    print LDOC "\@cindex ". $procs[$i] . "\n";
    if ($db_file && 
	(!defined($CHECKSUMS{$procs[$i]}) ||
	 $CHECKSUMS{$procs[$i]} != $chksum{$procs[$i]}))
    {
      $CHECKSUMS{$procs[$i]} = $chksum{$procs[$i]};
      $CHECKSUMS_CHANGED = 1;
    }
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
      $parsing = "help-string of $lib:$procs[$i]:";
      $ref = OutInfo(\*LDOC, $help{$procs[$i]});
      print LDOC "\@end table\n";
    }
    # print example
    if ($example{$procs[$i]} && 
	($ex = &CleanUpExample($lib, $example{$procs[$i]})))
    {
      print LDOC "\@strong{Example:}\n";
      print LDOC "\@smallexample\n\@c example$tag\n";
      print LDOC $ex;
      print LDOC "\n\@c example\n\@end smallexample\n";
    }
    OutRef(\*LDOC, $ref) if $ref;
    print LDOC "\@c ---end content $procs[$i]---\n";
  }
  # save checksums
  if ($CHECKSUMS_CHANGED)
  {
    open(CD, ">$db_file") || die "Can't open '$db_file' for writing: $!";
    print CD "%CHECKSUMS = (\n";
    for $key (keys %CHECKSUMS)
    {
      if ($CHECKSUMS{$key} == "sprintf")
      {
        print CD "q{$key}, \"sprintf\",\n";
      }
      else
      {
        print CD "q{$key}, $CHECKSUMS{$key},\n";
      }
    }
    print CD ");\n";
    close(CD);
  }
}

# 
# und Tschuess
#
if ($doc)
{
 print LDOC <<EOT;
\@c ----------------------------------------------------------
\@node Index, , Singular libraries, Top
\@chapter Index
\@printindex cp

\@bye
EOT
}
  
close(LDOC);
if ($error)
{
  print STDERR "ERROR: $error\n";
  exit(1);
}
exit(0);

###########################################################################
#
# parse and print-out libinfo
#
sub OutLibInfo
{
  my ($FH, $info, $l_fun) = @_;
  if ($info =~ /^\@/)
  {
    print $FH $info;
    return;
  }
  print $FH "\@table \@asis\n";
  $table_is_open = 1;
  
  my ($ref) = OutInfo($FH, $info, $l_fun);

  print $FH "\@end table\n" if $table_is_open;
  $table_is_open = 0;
  return $ref;
}

sub OutInfo
{
  my ($FH, $info, $l_fun) = @_;
  if ($info =~ /^\@/)
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
    if ($1 =~ /^([A-Z][A-Z0-9 ]+?):(.*\n)/)
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
  my ($pline, $line, $text, $ptext, $special);
  my ($in_format, $in_example, $in_texinfo);

  $length = 0 unless $length;
  $_ .= "\n";
  $ptext = $_;
  while ($ptext =~ /(.*)\n/g)
  {
    $pline = $line;
    $line = $1;
    # check whether we are in protected env
    if ($in_format || $in_example || $in_texinfo)
    {
      # end protected env?
      if ($line =~ /^\s*\@end (format|example|texinfo)\s*$/)
      {
	if ($in_format && $1 eq 'format')
	{
	  $in_format = 0;
	  $text .= "$line\n";
	}
	elsif ($in_example && $1 eq 'example')
	{
	  $in_example = 0;
	  $text .= "\@end smallexample\n";
	}
	elsif ($in_texinfo && $1 eq 'texinfo')
	{
	  $in_texinfo = 0;
	  $text .= "\n";
	}
	else
	{
	  $error = "While parsing $parsing: \@end $1 found without matching \@$1" unless $error;
	}
	next;
      }
      else
      {
	$text .= "$line\n";
	next;
      }
    }
    # look for @example, @format, @texinfo
    if ($line =~ /^\s*\@(example|format|texinfo)\s*$/)
    {
      $special = 1;
      if ($1 eq 'example')
      {
	$text .= "\@smallexample\n";
	$in_example = 1;
      }
      elsif ($1 eq 'format')
      {
	$text .= "$line\n";
	$in_format = 1;
      }
      else
      {
	$text .= "\n";
	$in_texinfo = 1;
      }
      next;
    }
    my $ref = 'ref';
    if ($line =~ /([^\@]|^)\@(code|math|xref|pxref|$ref){(.*?)}/)
    {
      my $l = $line;
      $l =~ s/^\s*//;
      $l =~ s/\s$//;
      while ($l =~ /([^\@]|^)\@(code|math|xref|pxref|$ref){(.*?)}/)
      {
	$text .= CleanAscii($`.$1);
	$text .= "\@$2\{$3\}";
	$l = $';
      }
      $special = 1;
      $text .= CleanAscii($l) . "\n";
      next;
    }
    # break line if
    $text .= '@*' 
      if ($line =~ /\w/ 
	  && $pline =~ /\w/	 # line and prev line are not empty
	  && $line !~ /^\s*\@\*/ # line does not start with @*
	  && $pline !~ /\@\*\s*/ # prev line does not end with @*
	  && length($pline) + $length < 60 # prev line is shorter than 60 chars
	  && ! $special);	 # prev line was not special line
    $line =~ s/^\s*//;
    $line =~ s/\s$//;
    $special = 0;
    $text .= CleanAscii($line) . "\n";
  }
  $_ = $text;
  s/^\s*//;
  s/\s*$//;
  $_ .= "\n";
  if ($in_format || $in_texinfo || $in_example)
  {
    $error = "While parsing $parsing: no matching \@end " .
      ($in_format ? "format" : ($in_texinfo ? "texinfo" : "example" )) .
	" found"
	  unless $error;
  }
}

sub CleanAscii
{
  my $a = shift;
  $a =~ s/(\@([^\*]|$))/\@$1/g; # escape @ signs, except @*, @{, @}
  $a =~ s/{/\@{/g; # escape {}
  $a =~ s/}/\@}/g;
  $a =~ s/\t/ /g;
  $a =~ s/ +/ /g;	   
  return $a;
}

sub OutInfoItem
{
  my ($FH, $item, $text, $l_fun) = @_;
  local $parsing  = $parsing . uc($item);

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
  elsif ($item =~ m/keywords/i || m/keyphrases/i)
  {
    # index entries
    return OutKeywords($FH, $text);
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
      if ($text !~ /[a-z]/)
      {
	print $FH lc $text;
      }
      else
      {
	print $FH $text;
      }
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
    print $FH "* ${proc}:: $_";
    push @procs, $proc;
  }
  else
  {
    print $FH "\@item \@code{$proc($procargs)}  ";
    print $FH "\n\@cindex $proc\n";
    print $FH "\@anchor{$proc}\n$_";
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
    print $FH "; \@ref{$ref}"  if ($ref =~ /\w/);
  }
  print $FH "\.\n\@c ref\n\n";
}

sub OutKeywords
{
  my ($FH, $kws) = @_;
  for $kw (split (/;/, $kws))
  {
    $kw =~ s/^\s*(.*?)\s*$/$1/;
    print $FH "\@cindex $kw\n";
  }
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
  $example = undef unless $example =~ /\w/;
  # prepend LIB command
  if ($so_module =~ /lib/)
  {
    $example = "LIB \"$lib.lib\";\n".$example 
      if ($example && $lib ne "standard");
  }
  else
  {
    $example = "LIB\(\"$lib.so\"\);\n".$example;
  }  
  # erase empty lines
  $example =~ s/^\s*\n//g;
  # erase spaces from beginning of lines
  $example =~ s/\n\s*/\n/g;
  $example =~ s/\s*$//g;
  return $example;
}

sub print_doc_header
{
  my $fh = shift;
  ($hlp_file = $out_file) =~ s/doc$/hlp/;
  print $fh <<EOT;
\\input texinfo   \@c -*-texinfo-*-
\@c %**start of header
\@setfilename $hlp_file
\@settitle Formatted manual of $lib.lib
\@paragraphindent 0
\@c %**end of header

\@ifinfo
This file documents contains the formatted documentation of $library
\@end ifinfo

\@titlepage
\@title Formatted manual of $library
\@end titlepage

\@node Top, , , (dir)

\@ifnottex
This file contains the formatted documentation of $library
\@end ifnottex

\@menu
* Singular libraries::
* Index::
\@end menu

\@node Singular libraries, Index,,Top
\@comment node-name,next, previous, up
\@chapter Singular libraries

\@menu
* ${lib}_lib::  
\@end menu
  
\@node ${lib}_lib,,,Singular libraries
\@section ${lib}_lib

\@example 
-------BEGIN OF PART WHICH IS INCLUDED IN MANUAL-----
\@end example


EOT
}
