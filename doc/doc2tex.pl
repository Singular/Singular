#!/usr/local/bin/perl
# $Id: doc2tex.pl,v 1.11 1999-07-21 19:53:23 obachman Exp $
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
# @c lib libname.lib[:proc] [lib_fun, lib_ex, no_ex]
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
$Singular_opts = " -teqr12345678";
$clean = 0;
$verbose = 1;
$reuse = 1;
$no_ex = 0;
$no_fun = 0;
$doc_subdir = "./d2t_singular";
@include_dirs = (".", "../Singular/LIB");
$doc_examples_db = "$doc_subdir/examples";
$make = 0;

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
  if (/^-o(output)?$/)   { $tex_file = shift(@ARGV); next;}
  if (/^-no_r(euse)?$/)  { $reuse = 0; next;}
  if (/^-c(lean)?$/)     { $clean = 1; next;}
  if (/^-no_e(x)?$/)     { $no_ex = 1; next;}
  if (/^-no_fu(n)?$/)    { $no_fun = 1;next;}
  if (/^-m(ake)?$/)      { $make = 1;next;}
  if (/^-s(ubdir)?$/)    { $doc_subdir = shift(@ARGV); next;}
  if (/^-I$/)            { unshift(@include_dirs, shift(@ARGV)); next;}
  if (/^-v(erbose)?$/)   { $verbose = shift(@ARGV); next;}
  if (/^-h(elp)?$/)      { print &Usage; exit;}
  Error("Unknown Option $_\n" .&Usage);
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
unless ($tex_file)
{
  $tex_file = ($no_ex ? "$doc_dir/${doc}_noEx.tex" : "$doc_dir/$doc.tex");
}

#
# open files
#
open(DOC, "<$doc_file") 
  || Error("can't open $doc_file for reading: $!\n" . &Usage);
open(TEX, ">$tex_file") || Error("can't open $tex_file for writing: $!\n");
print "d2t: Generating $tex_file from $doc_file ...\n" if ($verbose > 1);
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
  s/\s*$//;
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
    
    while (/\@ref{(.*?)}/)
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
  print TEX $header ? "$header\n" : "See also:\n";
  print TEX "\@ifinfo\n\@menu\n";
  foreach $ref (@refs) {print TEX "* ".$ref."::\n";}
  print TEX "\@end menu\n\@end ifinfo\n\@iftex\n";

  if ($header ne " ")
  {
    print TEX "$header\n" unless ($header eq " ");
  }
  else
  {
    print TEX "\@strong{See also:}\n";
  }
  $lref = pop(@refs);
  foreach $ref (@refs) {print TEX "\@ref{".$ref."};\n";}
  print TEX "\@ref{".$lref."}.\n" if ($lref);  
  print TEX "\@end iftex\n\@c end inserted refs from $doc_file:$lline\n";
}

###################################################################
#
# @c lib libname.lib[:proc] [no_ex, no_fun, (\w*)section]
#   --> replaced by @include $texfile where 
#        $texfile = $subdir/libname_lib[_noFun,_noEx].tex
#   --> if $make, calls "make $texfile" 
#   --> Error, if $tex_file does not exist
#   --> if [:proc] is given, then includes only of respective
#       proc body
#   --> if (\w*)section is given, replaces @subsection by @$1section
#       and pastes in content of tex file directly

sub HandleLib
{
  my($lib, $proc, $n_fun, $n_ex, $section, $tex_file);

  if (/^\@c\s*lib\s+([^\.]+)\.lib(.*)/)
  {
    $lib = $1;
    $_ = $2;
  }
  else
  {
    warn "$WARNING need .lib file to process '$_'\n";
    print TEX $_;
    return;
  }

  $proc = $1 if (/^:(.*?) /);
  $n_fun = 1 if ($no_fun || /no_fun/);
  $n_ex = 1 if ($no_ex || /no_ex/);
  $section = $1 if /\w*section/;
  
  # contruct tex file name
  $tex_file = "$doc_subdir/$lib"."_lib";
  if ($n_fun)
  {
    $tex_file .= "_noFun";
  }
  elsif ($n_ex)
  {
    $tex_file .= "_noEx";
  }
  $tex_file .= ".tex";

  if ($make)
  {
    print "<lib $lib" if ($verbose);
    if ($verbose <= 1)
    {
      System("make $tex_file > /dev/null") 
    }
    else
    {
      System("make $tex_file"); 
    }
  }
  
  # make sure file exists
  if (-r $tex_file)
  {
    if ($verbose)
    {
      print "<lib $lib" unless $make;
      print ":$proc" if $proc;
      print ">";
    }
  }
  else
  {
    Error("Can't read $tex_file\n") unless -r $tex_file;
  }

  # see whether we have to paste something in
  if ($proc || $section)
  {
    open(LTEX, "<$tex_file") 
      || Error("Can't open $tex_file for reading: $!\n");

    print TEX "\@c start include of docu for $lib.lib:$proc\n";
    print TEX "\@c replaced \@subsection by \@$section\n" if ($section);
    if ($proc)
    {
      my $found = 0;
      while (<LTEX>)
      {
	$found = 1 if /c ---content $proc---/;
	if ($found)
	{
	  s/subsection/$section/ if $section;
	  print TEX $_; 
	}
	last if $found && /c ---end content $proc---/;
      }
      if ($found)
      {
	Error("no end content found for lib proc docu for $lib.lib:$proc $doc_file:$line \n")
	  unless (/c ---end content $proc---/);
	print TEX "\@c generated lib proc docu for $lib.lib:$proc $doc_file:$line \n";
      }
      else
      {
	Error("did not find lib proc docu for $lib.lib:$proc $doc_file:$line \n");
      }
    }
    else
    {
      while (<LTEX>)
      {
	s/subsection/$section/;
	print TEX $_;
      }
    }
    print TEX "\@c end include of docu for $lib.lib:$proc\n";
    close(LTEX);
  }
  else
  {
    print TEX "\@c include of docu for $lib.lib\n";
    print TEX "\@include $tex_file\n";
  }
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
  -output file  : use 'file' as output file
                          (default: input_file.tex)
  -clean        : delete intermediate files
  -make         : use make to generate tex files for libraries
  -no_reuse     : don't reuse intermediate files
  -no_ex        : skip computation of examples 
  -no_fun       : don't include help for library functions
  -subdir  dir  : put intermediate files into 'dir'
                          (default: './d2t_singular')
  -I dir        : look also into 'dir' for include  and lib files
                          (default: ".", "../Singular/LIB")
  -verbose  val : Set verbosity to 'val' (0=quiet, 1=prot, >1=all)
  -help         : print help and exit
EOT
}

