#!/usr/local/bin/perl
# $Id: doc2tex.pl,v 1.7 1999-07-15 17:06:11 obachman Exp $
###################################################################
#  Computer Algebra System SINGULAR
#
#  doc2tex: utility to generate the Singular manual
#
####
# @c example [error]
#    -> the text till the next @c example is feed into Singular,
#       the text is then substituted by
#       @c computed example $example $doc_file:$line
#       <text from the input>
#       @expansion{} <corresponding output>
#       ....
#       @c end computed example $example $doc_file:$line
#       reuse computed examples if ($reuse && -r $example.inc)
#       cut absolute directory names from the loaded messages 
#       substituted @,{ and } by @@, @{ resp. @}
#       wrap around output lines  longer than $ex_length = 73;
#       Processing is aborted if error occures in Singular run, 
#       unless 'error' is specified 
#
####
# @c include file
#    -> copy content of file into output file protecting texi special chars
#
####
# @c ref
# ....
# @c ref
#    -> scans intermediate lines for @ref{..} strings
#    Creates menu of (sorted) refs for ifinfo
#    Creates comma-separated (sorted) refs for iftex, prepended with 
#    the text before first @ref.
#
####
# @c lib libname.lib[:proc] [no_ex, lib_fun, lib_ex]
#   Without :proc
#   --> includes info of libname.lib in output file
#   --> includes function names of info into function index
#   --> if lib_fun is given, includes listing of functions and 
#                      their help into output file
#   --> if lib_ex is given, includes computed examples of functions, as well
#   With :proc
#   --> includes content of procedure 'proc' from library libname:lib
#
#   Optional no_ex, lib_fun, lib_ex arguments overwrite respective
#    command-line arguments
# 
#
###################################################################

#
# default settings of command-line arguments
#
$Singular = "../Singular/Singular"; # should be overwritten
$libparse = "../Singular/libparse"; # change in final version
$Singular_opts = " -teqr12345678";
$clean = 0;
$verbose = 1;
$reuse = 1;
$no_ex = 0;
$lib_fun = 0;
$lib_ex = 0;
$doc_subdir = "./d2t_singular";
@include_dirs = (".", "../Singular/LIB");
$doc_examples_db = "$doc_subdir/examples";

#
# misc. defaults
#
$ex_length = 73;
$ERROR = "\n**** Error:";
$WARNING = "\n** Warning:";

#
# flush stdout and stderr after every write
#
select(STDERR);
$| = 1;
select(STDOUT);
$| = 1;

#
# get file to convert
#
$doc_file = pop(@ARGV);
if ($doc_file =~  /(.+)\..*$/)
{
  die "*** Error: Need .doc file as input\n" . &Usage  
    unless ($doc_file =~ /(.+)\.doc$/);
}
else
{
  if ($doc_file =~ /^-h(elp)?$/) { print &Usage; exit;}
  $doc_file .= ".doc";
}

#
# parse command line options
#
$args = join(" ", @ARGV);
while (@ARGV && $ARGV[0] =~ /^-/) 
{
  $_ = shift(@ARGV);
  if (/^-S(ingular)?$/)  { $Singular = shift(@ARGV); next;}
  if (/^-l(ibparse)?$/)  { $libparse = shift(@ARGV); next;}
  if (/^-o(output)?$/)   { $tex_file = shift(@ARGV); next;}
  if (/^-no_r(euse)?$/)  { $reuse = 0; next;}
  if (/^-c(lean)?$/)     { $clean = 1; next;}
  if (/^-no_e(x)?$/)     { $no_ex = 1; next;}
  if (/^-lib_fu(n)?$/)   { $lib_fun = 1;next;}
  if (/^-lib_e(x)?$/)    { $lib_ex = 1; next;}
  if (/^-s(ubdir)?$/)    { $doc_subdir = shift(@ARGV); next;}
  if (/^-I$/)            { unshift(@include_dirs, shift(@ARGV)); next;}
  if (/^-v(erbose)?$/)   { $verbose = shift(@ARGV); next;}
  if (/^-h(elp)?$/)      { print &Usage; exit;}
  die &Usage;
}
$verbose = ($verbose < 0 ? 0 : $verbose);

#
# construct filenames
# 
($doc_dir = $doc_file) =~ s/(.+)\.doc$/$1/;
if ($doc_dir =~ /(.*)\//)
{
  $doc = $';
  $doc_dir = $1;
}
else
{
  $doc = $doc_dir;
  $doc_dir = ".";
}
$tex_file = "$doc_dir/$doc.tex" unless ($tex_file);
	       
#
# open files
#
open(DOC, "<$doc_file") 
  || Error("can't open $doc_file for reading: $!\n" . &Usage);
open(TEX, ">$tex_file") || Error("can't open $tex_file for writing: $!\n");
print "d2t: Generating $tex_file from $doc_file ...\n" if ($verbose > 1);
print "d2t: $doc_file ==> $tex_file\n" if ($verbose == 1);
if (-d $doc_subdir)
{
  print "d2t: Using $doc_subdir for intermediate files\n"  
    if ($verbose > 1);
}
else
{
  mkdir($doc_subdir, oct(755)) 
    || Error("can't create directory $doc_subdir: $!\n");
  print "d2t: Created $doc_subdir for intermediate files\n"  
    if ($verbose > 1);
}

dbmopen(%EXAMPLES, $doc_examples_db, oct(755)) || die "$ERROR: can't open examples data base: $!\n";

#######################################################################
# 
# go !
#
while (<DOC>)
{
  $line++;
  
  if (/^\@c\s*example/)     {&HandleExample; next;}
  if (/^\@c\s*include\s+/)  {&HandleInclude; next;}
  if (/^\@c\s*ref\s*$/)     {&HandleRef; next;}
  if (/^\@c\s*lib\s+/)      {&HandleLib; next;}
  if (/^\@setfilename/)     {print TEX "\@setfilename $doc.hlp\n"; next;}
			     
  print TEX $_;

  if (/^\@bye$/)            {last;}
}

#
# wrap up
#
close(TEX);
dbmclose(%EXAMPLES);
print "\nd2t: Finished generation of $tex_file \n" if ($verbose > 1);
print "\n" if ($verbose == 1);


######################################################################
# @c example [error]
#    -> the text till the next @c example is feed into Singular,
#       the text is then substituted by
#       @c computed example $example $doc_file:$line
#       <text from the input>
#       @expansion{} <corresponding output>
#       ....
#       @c end computed example $example $doc_file:$line
#       reuse computed examples if ($reuse && -r $example.inc)
#       cut absolute directory names from the loaded messages 
#       substituted @,{ and } by @@, @{ resp. @}
#       wrap around output lines  longer than $ex_length = 73;
#       Processing is aborted if error occures in Singular run, 
#       unless 'error' is specified 
sub HandleExample
{
  my($lline, $thisexample, $include, $error_ok);
  
  $lline = $line;
  $example++;

  if ($no_ex)
  {
    print "{$example}" if ($verbose);
    print TEX "\@c skipped computation of example $example $doc_file:$lline \n";
    
  }

  $thisexample = '';
  $error_ok = 1 if /error/;
  # print content in example file till next @c example 
  while (<DOC>)
  {
    $line++;
    last if (/^\@c\s*example\s*$/);
    s/^\s*//; # remove preceeding white spaces
    if ($no_ex)
    {
      &protect_texi;
      print TEX $_;
    }
    else
    {
      $thisexample .= $_ unless (/^\s*$/);
    }
  }
  Error("no matching '\@c example' found for $doc_file:$lline\n")
    unless (/^\@c\s*example\s*$/);

  # done, if no examples
  return if ($no_ex);

  # check whether it can be reused
  $include = $EXAMPLES{$thisexample};
  if ($reuse && ($include = $EXAMPLES{$thisexample}))
  {
    print "<$example>" if ($verbose);
    print TEX "\@c reused example $example $doc_file:$lline \n";
  }
  else
  {
    print "($example" if ($verbose == 1);
    my ($ex_file, $res_file, $inc_file);
    $inc_file = "$doc_subdir/$doc"."_$example.inc";
    $ex_file = "$doc_subdir/$doc"."_$example.tst";
    $res_file = "$doc_subdir/$doc"."_$example.res";

    print TEX "\@c computed example $example $doc_file:$lline \n";

    # run singular
    open(EX, ">$ex_file") || Error("can't open $ex_file for writing: $!\n");
    print EX "$thisexample\$\n";
    close(EX);

    &System("$Singular $Singular_opts $ex_file > $res_file");
    print ")" if ($verbose == 1);

    open(RES, "<$res_file") || Error("can't open $res_file for reading: $!\n");
    open(INC, ">$inc_file") || Error("can't open $inc_file for writing: $!\n");

    $include = '';
    # get result, manipulate it and put it into inc file
    while (<RES>)
    {
      last if (/^$ex_file\s*([0-9]+)..\$/);
      # check for error
      Error("while running example $example from $doc_file:$lline.\nCall: '$Singular $Singular_opts $ex_file > $res_file'\n")
	if (/error occurred/ && ! $error_ok);
      # remove stuff from echo
      if (/^$ex_file\s*([0-9]+)../)
      {
	$_ = $';
	&protect_texi;
      }
      else
      {
	local($to_do, $done);
	# remove absolute path names from laoded messages
	s/^(\/\/ \*\* loaded )(.*)\/(.+).lib(.*)/$1$3.lib$4/;
	# shorten error occurred in messages
        s/\? error occurred in [^ ]* line/\? error occurred in line/;
	# break after $ex_length characters
	$to_do = $_;
	while (length($to_do) > $ex_length && $to_do =~ /\w/)
	{
	  $done .= substr($to_do, 0, $ex_length)."\\\n   ";
	  $to_do = substr($to_do, $ex_length + 1);
	}
	$_ = $done.$to_do if($done);
	&protect_texi;
	$_ = "\@expansion{} ".$_;
      }
      $include .= $_;
      print INC $_;
    }
    close(RES);
    close(INC);
    unlink $ex_file, $res_file, $inc_file if ($clean);
    $EXAMPLES{$thisexample} = $include;
  }
  print TEX $include;
  print TEX "\@c end example $example $doc_file:$lline\n";
}
  
######################################################################
# @c include file
#    -> copy content of file into output file protecting texi special chars
sub HandleInclude
{
  s/^\@c\s*include\s+([^\s]+)\s/$1/;
  unless (&Open(*INC, "<$_"))
  {
    warn "$WARNING HandleInclude: can't open $_ for reading\n";
    print TEX "\@c include file $_ not found\n";
    return;
  }
  print "<$_>" if ($verbose);
  print TEX "\@c begin included file $_ from $doc_file:$line\n";
  while (<INC>)
  {
    &protect_texi;
    print TEX $_;
  }
  print TEX "\@c end included file from $doc_file:$line\n";
  close (INC);
}

######################################################################
# @c ref
# ....
# @c ref
#    -> scans intermediate lines for @ref{..} strings
#    Creates menu of (sorted) refs for ifinfo
#    Creates comma-separated (sorted) refs for iftex, prepended with 
#    the text before first @ref.

sub HandleRef
{
  local(%refs, @refs, $header, $lline, $lref);
  
  print TEX "\@c inserted refs from $doc_file:$line\n";
  
  # scan lines use %ref to remove duplicates
  $lline = $line;
  while (<DOC>)
  {
    $line++;
    last if (/^\@c\s*ref\s*$/);
    
    while (/\@ref{([^\s]*)}/)
    {
      $refs{$1} = 1;
      $_ = $';
      unless ($header)
      {
	$header = $`;
	$header = " " unless ($header);
      }
    }
  }
  die "$ERRROR no matching \@c ref found for $doc_file:$lline\n" 
    unless (/^\@c\s*ref\s*$/);
  # sort refs
  @refs = sort(keys(%refs));
  # put them out
  print TEX "\@ifinfo\n\@menu\n";
  foreach $ref (@refs) {print TEX "* ".$ref."::\n";}
  print TEX "\@end menu\n\@end ifinfo\n\@iftex\n";

  if ($header ne " ")
  {
    print TEX "$header\n" unless ($header eq " ");
  }
  else
  {
    print TEX "\@strong{See also:} ";
  }
  $lref = pop(@refs);
  foreach $ref (@refs) {print TEX "\@ref{".$ref."};\n";}
  print TEX "\@ref{".$lref."}.\n" if ($lref);  
  print TEX "\@end iftex\n\@c end inserted refs from $doc_file:$lline\n";
}

###################################################################
#
# @c lib libname.lib [no_ex, lib_fun, lib_ex]
#   --> includes info of libname.lib in output file
#   --> includes function names of info into function index
#   --> if lib_fun is given, includes listing of functions and 
#                      their help into output file
#   --> if lib_ex is given, includes computed examples of functions, as well
# 
# Optional no_ex, lib_fun, lib_ex arguments overwrite respective
# command-line arguments
# 

sub HandleLib
{
  local($lib, $lib_name, $ltex_file, $l_ex, $l_fun);
  my ($func);

  if (/^\@c\s*lib\s+([^\.]+)\.lib(.*)/)
  {
    $lib = $1;
    $lib_name = $lib.".lib";
    $_ = $2;
  }
  else
  {
    warn "$WARNING need .lib file to process '$_'\n";
    print TEX $_;
    return;
  }

  $func = $1 if (/^:(.*?) /);
  $l_fun = 1 if (($lib_fun || (/lib_fun/)) && !/no_fun/);
  $l_ex = 1 if (($lib_ex || /lib_ex/) && !/no_ex/ && $l_fun);

  $ltex_file = "$doc_subdir/$lib"."_lib";
  unless ($l_ex)
  {
    if ($l_fun)
    {
      $ltex_file .= "_noEx";
    }
    else
    {
      $ltex_file .= "_noFun";
    }
  }
  $ltex_file .= ".tex";
  
  Error("can't open $lib.lib for reading: $!\n") 
    unless  ($lib_dir = &Open(*LIB, "<$lib.lib"));
  close (LIB);
  if ($reuse && open(LTEX, "<$ltex_file") && 
      IsNewer($ltex_file, "$lib_dir/$lib.lib"))
  {
    unless ($func)
    {
      print "<lib $lib>" if ($verbose);
      print TEX "\@c reused lib docu for $lib_name $doc_file:$line \n";
    }
  }
  elsif (&GenerateLibDoc($lib, $ltex_file, $l_fun, $l_ex))
  {
    print TEX "\@c generated lib docu for $lib_name $doc_file:$line \n"
      unless $func;
    open(LTEX, "<$ltex_file") 
      || Error("can't open $ltex_file for reading: $!\n");
  }
  if ($func)
  {
    print "<$lib:$func" if $verbose;
    print TEX "\@c generated lib proc docu for $lib_name:$func $doc_file:$line \n";
    my $found = 0;
    while (<LTEX>)
    {
      $found = 1 if /c ---content $func---/;
      print TEX $_ if $found;
      last if $found && /c ---end content $func---/;
    }
    if ($found)
    {
      Error("no end content found for lib proc docu for $lib_name:$func $doc_file:$line \n")
	unless (/c ---end content $func---/);
      print TEX "\@c generated lib proc docu for $lib_name:$func $doc_file:$line \n";
    }
    else
    {
      Error("did not find lib proc docu for $lib_name:$func $doc_file:$line \n");
    }
    print ">" if $verbose;
  }
  else
  {
    while (<LTEX>) {print TEX $_;}
    print TEX "\@c end generated lib docu for $lib_name $doc_file:$line \n";
  }
  close(LTEX);
  unlink $ltex_file if ($clean);
}
 
sub GenerateLibDoc
{
  my($lib, $tex_file, $l_fun, $l_ex) = @_;
  my($lib_dir, $scall, $pl_file, $doc_file, $i, $example,$largs, $ref);
  # vars from executing the library perl scrip
  local($info, $libary, $version, @procs, %example, %help, $table_is_open);
  
  print "(lib $lib: " if ($verbose == 1);
  # construct doc/tex file name
  $doc_file = "$doc_subdir/$lib"."_lib";
  $doc_file .= "_noFun" unless ($l_fun);
  $doc_file .= ".doc";

  Error("can't open $lib.lib for reading: $!\n")
    unless  ($lib_dir = &Open(*LIB, "<$lib.lib"));
  close (LIB);
  if (-r $doc_file && $reuse && IsNewer($doc_file, "$lib_dir/$lib.lib"))
  {
    print "<doc>" if ($verbose == 1);
  }
  else
  {
    # generate perl file, if necessary
    $pl_file = "$doc_subdir/$lib"."_lib.pl";
    if (-r $pl_file && $reuse && IsNewer($pl_file, "$lib_dir/$lib.lib"))
    {
      print "<pl>" if ($verbose == 1);
    }
    else
    {
      print "(pl" if ($verbose == 1);
      &System("$libparse -i $lib_dir/$lib.lib > $pl_file");
      print ")" if ($verbose == 1);
    }

    print "(doc" if ($verbose == 1);
    
    do $pl_file;
    Error("error while executing $pl_file: $@\n") if ($@);
    unlink ($pl_file) if ($clean);
    
    # generate doc file
    open(LDOC, ">$doc_file") || die"$ERROR can't open $doc_file for writing: $!\n";
    
    # print header
    print LDOC "\@c library version: $version\n";
    print LDOC "\@c library file: $library\n";
    undef @procs;
    $ref = OutLibInfo(\*LDOC, $info, $l_fun);
    OutRef(\*LDOC, $ref) if $ref;
    # print menu of available functions 
    if ($l_fun)
    {
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

	print LDOC "\@c ---content $procs[$i]---\n";
	print LDOC "Procedure from library \@code{$lib.lib} (\@pxref{${lib}_lib}).\n\n";
	print LDOC "\@table \@asis\n";
	$table_is_open = 1;
	# print help
	$ref = OutInfo(\*LDOC, $help{$procs[$i]});
	print LDOC "\@end table\n";
	$table_is_open = 0;
	# print example 
	if ($example = &CleanUpExample($lib, $example{$procs[$i]}))
	{
	  print LDOC "\@strong{Example:}\n";
	  print LDOC "\@smallexample\n\@c example\n";
	  print LDOC $example;
	  print LDOC "\n\@c example\n\@end smallexample\n";
	}
	OutRef(\*LDOC, $ref) if $ref;
	print LDOC "\@c ---end content $procs[$i]---\n";
      }
    }
    close(LDOC);
    print ")" if ($verbose == 1);
  }

  $largs = "-no_ex" unless ($l_ex);
  # call doc2tex on generated doc file
  print "\n" if ($verbose == 1);
  dbmclose(%EXAMPLES);
  &System("$0 $args $largs -o $tex_file $doc_file");
  dbmopen(%EXAMPLES, $doc_examples_db, oct(755)) 
    || die "$ERROR: can't open examples data base: $!\n";
  
  unlink($doc_file) if ($clean);
  return 1;
}

###########################################################################
# parse and print-out libinfo
sub OutLibInfo
{
  my ($FH, $info, $l_fun) = @_;
  print $FH "\@c ---content LibInfo---\n";
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
      $text .= '@*' 
	if ($line =~ /\w/ && $pline =~ /\w/ && 
	    ((length($pline) < 60) || $line =~ /^\s*\w*\(.*?\)/));
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
  s/\@\*\s*/\@\*/g;
  s/(\@[^\*])/\@$1/g; # escape @ signs, except @*
  s/{/\@{/g;
  s/}/\@}/g;
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
    # if functions are in text, then make it in code
    s/(\w+\(.*?\))/\@code{$1}/g
      if ($item =~ /usage/i || $item =~ /Return/i);

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
  my @refs = split (/[\s,]+/, $refs);
  my $ref;

  print $FH "\@c ref\nSee\n";
  $ref = shift @refs;
  print $FH "\@ref{$ref}";
  for $ref (@refs)
  {
    print $FH ", \@ref{$ref}";
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

###################################################################
#
# Auxiallary functions
# 
sub IsNewer
{
  my $f1 = shift;
  my $f2 = shift;
  my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime)
    = stat($f1);
  my $m1 = $mtime;
  ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime) = stat($f2);

  return ($m1 > $mtime);
}

####################################################################
# Auxillary routines
#

# protect texi special characters
sub protect_texi
{
  s/\@/\@\@/g;
  s/{/\@{/g;
  s/}/\@}/g;
}	 

# open w.r.t. include_dirs
sub Open
{
  local(*FH, $file) = @_;
  local($mode);
  $file =~ s/^(.{1})(.*)/$2/;
  $mode = $1;

  foreach $dir (@include_dirs)
  {
    return $dir if(open(FH, $mode.$dir."/".$file));
  }
}
    
# system call with echo on verbose > 1 and die on fail
sub System
{
  local($call) = @_;
  print "\nd2t system: $call\n" if ($verbose > 1);
  Error("non-zero exit status of system call: '$call': $!\n")
    if (system($call));
}

sub Error
{
  print "$ERROR $_[0]";
  close(TEX);
  unlink $tex_file if $tex_file && -e $tex_file;
  exit(1);
}

#
# leave this here --otherwise fontification in my emacs gets screwd up
# 
sub Usage
{
  return <<EOT;
This is doc2tex: a utility to generate Singular texinfo from doc file
To convert a doc file to texinfo: $0 [options] input_file.doc
where options can be (abbreviated to shortest possible prefix):
  -Singular prog: use 'prog' as Singular program to generate ex output
                          (default: '../Singular/Singular')
  -libparse prog: use 'prog' as libparse program to generate lib docu
                          (default: '../Singular/libparse')
  -output file  : use 'file' as output file
                          (default: input_file.tex)
  -clean        : delete intermediate files
  -no_reuse     : don't reuse intermediate files
  -no_ex        : skip computation of examples 
  -lib_fun      : include help for library functions
  -lib_ex       : include example for library functions
  -subdir  dir  : put intermediate files into 'dir'
                          (default: './d2t_singular')
  -I dir        : look also into 'dir' for include  and lib files
                          (default: ".", "../Singular/LIB")
  -verbose  val : Set verbosity to 'val' (0=quiet, 1=prot, >1=all)
  -help         : print help and exit
EOT
}

