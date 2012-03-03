#!/usr/local/bin/perl
# $Id$
###################################################################
#  Computer Algebra System SINGULAR
#
#  doc2tex: utility to generate the Singular manual
#
####
# @c example [error] [no_comp] [unix_only]
#    -> the text till the next @c example is feed into Singular,
#       the text is then substituted by
#       @c computed example $ex_prefix $doc_file:$line
#       <text from the input>
#       @expansion{} <corresponding output>
#       ....
#       @c end computed example $ex_prefix $doc_file:$line
#       reuse computed examples if ($reuse && -r $ex_prefix.inc)
#       cut absolute directory names from the loaded messages 
#       substituted @,{ and } by @@, @{ resp. @}
#       wrap around output lines  longer than $ex_length = 73;
#       Processing is aborted if error occures in Singular run, 
#       unless 'error' is specified
#       if no_comp is given, then computation is not run
#       if unix_only is given, then computation is only run
#                              under unix
#       
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
# @c lib libname.lib[:proc] [no_ex, no_fun, Fun, (\w*)section]
#   --> replaced by @include $texfile where 
#        $texfile = $subdir/libname_lib[_noFun,_noEx].tex
#   --> if $make, calls "make $texfile" 
#   --> Error, if $tex_file does not exist
#   --> if [:proc] is given, then includes only of respective
#       proc body
#   --> Fun overwrites global no_fun
#   --> if (\w*)section is given, replaces @subsubsection by @$1section
#       and pastes in content of tex file directly
# 
#
###################################################################

use Config;
$Win32 = 1 if ($Config{osname} =~ /win/i);

#
# default settings of command-line arguments
#
$Singular = "../Singular/Singular"; # should be overwritten
$Singular_opts = " -teqr12345678 --no-rc";
$clean = 0;
$verbose = 1;
$reuse = 1;
$no_ex = 0;
%exclude_ex = ();
$no_fun = 0;
$doc_subdir = "./d2t_singular";
$ex_subdir = "./examples";
@include_dirs = (".", "../Singular/LIB");
$make = 0;
$make_opts = " --no-print-directory";


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
  if (/^-o(utput)?$/)    { $tex_file = shift(@ARGV); next;}
  if (/^-no_r(euse)?$/)  { $reuse = 0; next;}
  if (/^-c(lean)?$/)     { $clean = 1; next;}
  if (/^-no_e(x)?$/)     { $no_ex = 1; next;}
  if (/^-exclude$/)      { $exclude_ex{shift(@ARGV)} = 1;next;}
  if (/^-no_fu(n)?$/)    { $no_fun = 1;next;}
  if (/^-m(ake)?$/)      { $make =  shift(@ARGV);next;}
  if (/^-d(ocdir)?$/)    { $doc_subdir = shift(@ARGV); next;}
  if (/^-e(xdir)?$/)     { $ex_subdir = shift(@ARGV); next;}
  if (/^-I$/)            { unshift(@include_dirs, shift(@ARGV)); next;}
  if (/^-v(erbose)?$/)   { $verbose = shift(@ARGV); next;}
  if (/^-h(elp)?$/)      { print &Usage; exit;}
  Error("Unknown Option $_\n" .&Usage);
}
$verbose = ($verbose < 0 ? 0 : $verbose);
$make_opts .= " -s" unless $verbose > 1;

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
print "(d2t $doc_file==>$tex_file" if ($verbose);
if (-d $doc_subdir)
{
  print "<docdir:$doc_subdir>"  if ($verbose > 1);
}
else
{
  mkdir($doc_subdir, oct(755)) 
    || Error("can't create directory $doc_subdir: $!\n");
  print "(docdir:$doc_subdir)"  if ($verbose > 1);
}
if (-d $ex_subdir)
{
  print "<exdir:$ex_subdir>"  if ($verbose > 1);
}
else
{
  mkdir($ex_subdir, oct(755)) 
    || Error("can't create directory $ex_subdir: $!\n");
  print "(exdir:$ex_subdir)"  if ($verbose > 1);
}

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

  if (/^\@\w*section\s*/ || /^\@\w*chapter\s*/ || /^\@\w*heading\s*/)
  {
    $section = $';
    $section =~ s/^\s*(.*)\s*$/$1/;
    if ($section =~ /\(/)
    {
      $section =~ s/\(/_/g
    }
    if ($section =~ /\)/)
    {
      $section =~ s/\)/_/g
    }
    if ($section =~ /\w/)
    {
      $section =~ s/\s/_/g
    }
    else
    {
      $section = "unknown";
    }
  }

  # handle math
  $in_info = 1 if /^\@ifinfo/;
  $in_info = 0 if /^\@end\s*ifinfo/;
  s[\@math\{(.*?)\}][&HandleMath($1)]eg unless $in_info;

  print TEX $_;

  if (! $printed_header && /^\@c/) 
  {
    $printed_header = 1;
    print TEX <<EOT;
\@comment This file was generated by doc2tex.pl from $doc_file
\@comment DO NOT EDIT DIRECTLY, BUT EDIT $doc_file INSTEAD
EOT
  }

  if (/^\@bye$/)            {last;}
}

#
# wrap up
#
close(TEX);
print "==>$tex_file)\n" if ($verbose);

sub HandleMath
{
  my $what = shift;
  return <<EOT;

\@ifinfo
\@math{$what}
\@end ifinfo
\@tex
\$$what\$
\@end tex
EOT
}

######################################################################
# @c example [error] [no_comp] [unix_only]
#    -> the text till the next @c example is fed into Singular,
#       the text is then substituted by
#       @c computed example $ex_prefix $doc_file:$line
#       <text from the input>
#       @expansion{} <corresponding output>
#       ....
#       @c end computed example $ex_prefix $doc_file:$line
#       reuse computed examples if ($reuse && -r $ex_prefix.inc)
#       cut absolute directory names from the loaded messages 
#       substituted @,{ and } by @@, @{ resp. @}
#       wrap around output lines  longer than $ex_length = 73;
#       Processing is aborted if error occures in Singular run, 
#       unless 'error' is specified
#       If [no_comp] is given, actual computation is not run
#       if [unix_only] is given, then computation is only run
#                                under unix

sub HandleExample
{
  my($inc_file, $ex_file, $lline, $thisexample, $error_ok, $cache, $no_comp,
     $unix_only, $tag);
  
  $lline = $line;
  $section = 'unknown' unless $section;
  $ex_prefix = $section;
  $ex_prefix .= "_$examples{$section}" if $examples{$section};
  $examples{$section}++;

  if (/tag:(\w+)/)
  {
    $tag = $1;
  }
  else
  {
    $tag = 'NOTAG';
  }
  
  if ($no_ex or $exclude_ex{$tag})
  {
    print "{$ex_prefix}" if ($verbose);
    print TEX "\@c skipped computation of example $ex_prefix $doc_file:$lline \n";
    
  }
  else
  {
    $inc_file = "$ex_subdir/$ex_prefix.inc";
    $ex_file = "$ex_subdir/$ex_prefix.sing";
    if (-r $inc_file && -r $ex_file && -s $inc_file && -s $ex_file)
    {
      $cache = 1;
      open(SING, "<$ex_file") || ($cache = 0);
    }
  }

  $thisexample = '';
  $error_ok = 1 if /error/;
  $no_comp = 1 if /no_comp/;
  $unix_only = 1 if /unix_only/ && $Win32;
  
  # print content in example file till next @c example
  print TEX "// only supported on Unix platforms\n"
    if $unix_only;
  
  while (<DOC>)
  {
    $line++;
    last if (/^\@c\s*example\s*$/);
#    s/^\s*//; # remove preceeding white spaces
    if ($no_ex || $exclude_ex{$tag} || $no_comp || $unix_only)
    {
      &protect_texi;
      print TEX $_;
    }
    else
    {
      $thisexample .= $_;
      if ($cache && $_ && $_ ne <SING>)
      {
	$cache = 0;
	close(SING);
      }
    }
  }
  close(SING) if $cache;
  Error("no matching '\@c example' found for $doc_file:$lline\n")
    unless (/^\@c\s*example\s*$/);

  # done, if no examples
  return if ($no_ex || $exclude_ex{$tag} || $no_comp || $unix_only);

  # check whether it can be reused
  if ($reuse && $cache)
  {
    my $ok = 1;
    print "<$ex_prefix>" if ($verbose);
    print TEX "\@c reused example $ex_prefix $doc_file:$lline \n";
    open(INC, "<$inc_file") || Error("can't open $inc_file for reading: $!\n");
    while (<INC>)
    {
      if (/error occurred/ && $ok && !error_ok)
      {
	Warn("Result file $inc_file contains errors. Force re-computation by removing $inc_file");
	$ok = 0;
      }
      print TEX $_;
    }
    close(INC);
  }
  else
  {
    print "($ex_prefix" if ($verbose == 1);
    my ($res_file);
    $res_file = "$ex_subdir/$ex_prefix.res";

    print TEX "\@c computed example $ex_prefix $doc_file:$lline \n";

    # run singular
    open(EX, ">$ex_file") || Error("can't open $ex_file for writing: $!\n");
    print EX "$thisexample";
    close(EX);

    unless ($Singular_OK)
    {
      if (system("echo '\$' | $Singular $Singular_opts > $res_file"))
      {
	$Singular .= '.exe' if ($Win32 && $Singular !~ /\.exe$/);
	Error("Can´t run '$Singular $Singular_opts': $@")
	  if (system("echo '\$' | $Singular $Singular_opts > $res_file"));
      }
      $Singular_OK = 1
    }

    &System("echo '\$' | $Singular $Singular_opts $ex_file > $res_file");
    print ")" if ($verbose == 1);

    open(RES, "<$res_file") || Error("can't open $res_file for reading: $!\n");
    open(INC, ">$inc_file") || Error("can't open $inc_file for writing: $!\n");

    # get result, manipulate it and put it into inc file
    while (<RES>)
    {
      last if (/^STDIN\s*([0-9]+)..\$/);
      # check for error
      Error("while running example $ex_prefix from $doc_file:$lline.\nCall: '$Singular $Singular_opts $ex_file > $res_file'\n")
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
	while (length($to_do) > $ex_length && $to_do =~ /\w/ &&  
	       substr($to_do, $ex_length) !~ /^\s*$/)
	{
	  
	  $done .= substr($to_do, 0, $ex_length)."\\\n   ";
	  $to_do = substr($to_do, $ex_length);
	}
	$_ = $done.$to_do if($done);
	&protect_texi;
	$_ = "\@expansion{} ".$_;
      }
      print INC $_;
      print TEX $_;
    }
    close(RES);
    close(INC);
    unlink $ex_file, $res_file, $inc_file if ($clean);
  }
  print TEX "\@c end example $ex_prefix $doc_file:$lline\n";
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
    
    while (/\@ref{(.*?)}[;\.]/)
    {
      $refs{$1} = 1;
      $_ = $';
      unless ($header)
      {
	$header = $`;
	$header = " " unless ($header);
      }
    }
    $header = $_ unless ($header)
  }
  chomp $header;
  die "$ERRROR no matching \@c ref found for $doc_file:$lline\n" 
    unless (/^\@c\s*ref\s*$/);
  # sort refs
  @refs = sort(keys(%refs));
  # put them out
  print TEX "\@ifinfo\n";
  print TEX "\@menu\n";
  if ($header && $header ne " ")
  {
    print TEX "$header\n";
  }
  else
  {
    print TEX "See also:\n";
  }
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
# @c lib libname.lib[:proc] [no_ex, no_fun, Fun, (\w*)section]
#   --> replaced by @include $texfile where 
#        $texfile = $subdir/libname_lib[_noFun,_noEx].tex
#   --> if $make, calls "make $texfile" 
#   --> Error, if $tex_file does not exist
#   --> if [:proc] is given, then includes only of respective
#       proc body
#   --> Fun overwrites global no_fun
#   --> if (\w*)section is given, replaces @subsubsection by @$1section
#       and pastes in content of tex file directly

sub HandleLib
{
  my($lib, $proc, $n_fun, $n_ex, $section, $tex_file, $tag);

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
  if (/tag:(\w+)/)
  {
    $tag = "TAG='-tag ".$1."'";
  }
  else
  {
    $tag = '';
  }

  $proc = $1 if (/^:(.*?) /);
  $n_fun = 1 if ($no_fun || /no_fun/);
  $n_fun = 0 if (/Fun/);
  $n_ex = 1 if ($no_ex || /no_ex/ || (/unix_only/ && $Win32));
  $section = $1 if /(\w*)section/;
  
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
    print "<lib $lib " if ($verbose);
    System("$make $make_opts $tag VERBOSE=$verbose $tex_file"); 
  }
  
  # make sure file exists
  if (-r $tex_file)
  {
    if ($verbose)
    {
      print "<lib $lib " unless $make;
      print "$proc>";
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
    print TEX "\@c replaced \@subsubsection by \@$section\n" if ($section);
    if ($proc)
    {
      my $found = 0;
      while (<LTEX>)
      {
	$found = 1 if /c ---content $proc---/;
	if ($found)
	{
	  s/subsubsection/${section}section/ if $section;
	  print TEX $_; 
	}
	last if $found && /c ---end content $proc---/;
      }
      if ($found)
      {
	Error("no end content found for lib proc docu for $lib.lib:$proc $doc_file:$line in $tex_file\n")
	  unless (/c ---end content $proc---/);
	print TEX "\@c generated lib proc docu for $lib.lib:$proc $doc_file:$line \n";
      }
      else
      {
	Error("did not find lib proc docu for $lib.lib:$proc $doc_file:$line in $tex_file\n");
      }
    }
    else
    {
      while (<LTEX>)
      {
	s/subsubsection/${section}section/;
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
  print " d2t system:\n$call\n" if ($verbose > 1);
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

sub Warn
{
  print "$WARNING $_[0]\n";
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
  -make  cmd    : use cmd as make command to generate tex files for libraries
  -no_reuse     : don't reuse intermediate files
  -no_ex        : skip computation of examples 
  -no_fun       : don't include help for library functions
  -docdir  dir  : put intermediate doc/tex files into 'dir'
                          (default: './d2t_singular')
  -exdir   dir  : put example files into 'dir'
                          (default: './examples')
  -I dir        : look also into 'dir' for include  and lib files
                          (default: ".", "../Singular/LIB")
  -verbose  val : Set verbosity to 'val' (0=quiet, 1=prot, >1=all)
  -help         : print help and exit
EOT
}

