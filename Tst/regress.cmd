#!/usr/bin/perl

#################################################################
# $Id: regress.cmd,v 1.12 1998-06-10 15:18:29 obachman Exp $
# FILE:    regress.cmd 
# PURPOSE: Script which runs regress test of Singular
# CREATED: 2/16/98
# AUTHOR:  obachman@mathematik.uni-kl.de

#################################################################
# 
# usage
# 
sub Usage
{
  print <<_EOM_
Usage:
regress.cmd    -- regress test of Singular
  [-s <Singular>]  -- use <Singular> as executable to test
  [-h]             -- print out help and exit
  [-k]             -- keep result (*.res) files, do not zip original res file
  [-v num]         -- set verbosity to num (used range 0..3, default: 1)
  [-g]             -- generate result (*.res.gz.uu) files, only
  [file.lst]       -- read tst files from file.lst
  [file.tst]       -- test Singular script file.tst 
_EOM_
}

#################################################################
# 
# the default settings
#
$singularOptions = "-teqr12345678";
$keep = "no";
$verbosity = 1;
$generate = "no";
$exit_code = 0;
chop($curr_dir=`pwd`);
# singular -- use the one in curr directory or the one found above
$singular = "$curr_dir/Singular";
if ( (! (-e $singular)) || (! (-x $singular)))
{
  $singular = $curr_dir."/../Singular";
}
# sed scripts which are applied to res files before they are diff'ed
$sed_scripts = "-e '/^\\/\\/.*used time:/d' -e '/^\\/\\/.*ignore:/d' -e '/error occured in/d'";


#################################################################
# 
# used programs
#
$diff = "diff";
$gunzip = "gunzip";
$gzip = "gzip";
$rm = "rm";
$mv = "mv";
$cp = "cp";
$tr = "tr";
$sed = "sed";
$cat = "cat";
$tee = "tee";
$WINNT = 1 if (system("uname -a | grep CYGWIN") == 0);;
if ($WINNT)
{
  $uudecode = "uudeview.exe -i";
}
else
{
  $uuencode = "uuencode";
  $uudecode = "uudecode";
}


#################################################################
# 
# auxiallary routines
# 
sub mysystem
{
  local($call) = $_[0];
  local($exit_status);
  
  print "$call\n" if ($verbosity > 1);
  return (system $call);
}

sub Set_withMP
{
  if (! $withMP)
  {
    $withMP = "no";
    &mysystem("$singular -qt -c 'system(\"with\", \"MP\");\$' > withMPtest");
    if (open(MP_TEST, "<withMPtest"))
    {
      $_ = <MP_TEST>;
      $withMP = "yes" if (/^1/);
      close(MP_TEST);
    }
    &mysystem("$rm -f withMPtest");
  }
}
    
    
sub MPok
{
  local($root) = $_[0];
  
  if (! open(TST_FILE, "<$root.tst"))
  {
    print (STDERR "Can not open $root.tst for reading\n");
    return (0);
  }
  while (<TST_FILE>)
  {
    if (/\"MP.+:.*\"/)
    {
      &Set_withMP;
      return (0) if ($withMP eq "no");
    }
  }
  return (1);
}
 
sub Diff
{
  local($root) = $_[0];
  local($exit_status);
  
  # prepare the result files: 
  &mysystem("$cat $root.res | $tr \"\\r\" \" \" | $sed $sed_scripts > $root.res.cleaned");
  &mysystem("$cat $root.new.res | $tr \"\\r\" \" \" | $sed $sed_scripts > $root.new.res.cleaned");

  # doo the diff call
  if ($verbosity > 0 && ! $WINNT)
  {
    $exit_status = &mysystem("$diff -w $root.res.cleaned $root.new.res.cleaned | $tee $root.diff");
  }
  else
  {
    $exit_status = &mysystem("$diff -w $root.res.cleaned $root.new.res.cleaned > $root.diff 2>&1");
  }
  
  # clean up time
  &mysystem("$rm -f $root.res.cleaned $root.new.res.cleaned");
  
  # there seems to be a bug here somewhere: even if diff reported
  # differenceses and exited with status != 0, then system still
  # returns exit status 0. Hence we manually need to find out whether
  # or not differences were reported: 
  # iff diff-file exists and has non-zero size
  $exit_status = $exit_status || (-e "$root.diff" && -s "$root.diff");

  return($exit_status);
}
  
sub tst_check
{
  local($root) = $_[0];
  local($system_call, $exit_status, $ignore_pattern);

  print "--- $root\n" unless ($verbosity == 0);
  # check for existence/readablity of tst and res file
  if (! (-r "$root.tst"))
  {
    print (STDERR "Can not read $root.tst\n");
    return (1);
  }

  # ignore MP stuff, if this singular does not have MP
  if (! &MPok($root))
  {
    print "Warning: $root not tested: needs MP\n";
    return (0);
  }

  # generate $root.res
  if ($generate ne "yes")
  {
    if ((-r "$root.res.gz.uu") && ! ( -z "$root.res.gz.uu"))
    {
      $exit_status = &mysystem("$uudecode $root.res.gz.uu; $gunzip -f $root.res.gz");
      if ($exit_status)
      {
	print (STDERR "Can not decode $root.res.gz.uu\n");
	return ($exit_status);
      }
    }
    elsif (! (-r "$root.res") || ( -z "$root.res"))
    {
      print (STDERR "Can not read $root.res[.gz.uu]\n");
      return (1);
    }
  }
  
    
  # prepare Singular run
  if ($verbosity > 2 && !$WINNT)
  {
    $system_call = "$cat $root.tst | $singular $singularOptions | $tee $root.new.res";
  }
  else
  {
    $system_call = "$cat $root.tst | $singular $singularOptions > $root.new.res 2>&1";
  }
  # Go Singular, Go!
  $exit_status = &mysystem($system_call);
  
  # prepare diff call 
  &mysystem("$rm -f $root.diff");
  if ($generate eq "yes")
  {
    if ($exit_status == 0) 
    {
      &mysystem("$cp $root.new.res $root.res");
    }
  }
  else 
  {
    # call Diff
    $exit_status = &Diff($root) || $exit_status;
  }
  
  # complain even if verbosity == 0
  if ($exit_status && $verbosity == 0)
  {
    print (STDERR "!!! $root\n");
  }

  #time to clean up
  if ($keep eq "no" && $exit_status == 0 && $generate ne "yes")
  {
    &mysystem("$rm -rf $root.new.res $root.diff");
    &mysystem("$rm -rf $root.res") if (-r "$root.res.gz.uu")
  } 
  elsif ($generate eq "yes" && $exit_status == 0)
  {
    if (! $WINNT)
    {
      &mysystem("$gzip -f $root.res; $uuencode $root.res.gz $root.res.gz > $root.res.gz.uu; $rm -rf $root.res.gz $root.diff");
    }
    else
    {
      # uuencode is broken under windows
      print "Warning: Can not generate $root.res.gz.uu under Windows\n";
    }
    if ($keep eq "yes")
    {
      &mysystem("$mv $root.new.res $root.res");
    }
    else
    {
      &mysystem("$rm -f $root.new.res");
    }
  }
  
  # und tschuess
  return ($exit_status);
}


#################################################################
# 
# Main program
# 

# process switches
while ($ARGV[0] =~ /^-/)
{
  $_ = shift;
  if (/^-s$/)
  {
    $singular = shift;
  }
  elsif (/^-h$/)
  {
    &Usage && exit (0);
  }
  elsif (/^-k$/)
  {
    $keep = "yes";
  }
  elsif (/^-g$/)
  {
    $generate = "yes";
  }
  elsif(/^-v$/)
  {
    $verbosity = shift;
  }
  else 
  {
    print (STDERR "Unrecognised option: $_\n") && &Usage && die;
  }
}

# if no command line arguments are left, use regress.lst
if ($#ARGV == -1)
{
  $ARGV[0] = "regress.lst";
}

# make sure $singular exists and is executable
$singular = "$curr_dir/$singular" unless ($singular =~ /\/.*/);

if ( ! (-e $singular))
{
  print (STDERR "Can not find $singular \n") && &Usage && die;
}
if (! (-x $singular))
{
  print (STDERR "Can not execute $singular \n") && &Usage && die;
}
if (-d $singular)
{
  print (STDERR "$singular is a directory\n") && &Usage && die;
}


# now do the work
foreach (@ARGV)
{

  if ( /^(.*)\.([^\.\/]*)$/ ) 
  {
    $_ = $1;
    $extension = $2;
  }

  if ( /^(.*)\/([^\/]*)$/ ) 
  {
    $path = $1;
    $base = $2;
    chdir($path);
    print "cd $path\n" if ($verbosity > 1);
  } 
  else 
  {
    $path = "";
    $base = $_;
  }
  $file = "$base.$extension";
  chop ($tst_curr_dir = `pwd`);
  
  if ($extension eq "tst")
  {
    $exit_code = &tst_check($base) || $exit_code;
  }
  elsif ($extension eq "lst")
  {
    if (! open(LST_FILE, "<$file"))
    {
      print (STDERR "Can not open $path/$file for reading\n");
      $exit_code = 1;
      next;
    }
    while (<LST_FILE>)
    {
      if (/^;/)          # ignore lines starting with ;
      {
	print unless ($verbosity == 0);
	next;
      }
      next if (/^\s*$/); #ignore whitespaced lines
      chop if (/\n$/);   #chop of \n
      
      $_ = $1 if (/^(.*)\.([^\.\/]*)$/ ); # chop of extension
      if ( /^(.*)\/([^\/]*)$/ ) 
      {
	$tst_path = $1;
	$tst_base = $2;
        chdir($tst_path);
	print "cd $tst_path\n" if ($verbosity > 1);
      } 
      else 
      {
	$tst_path = "";
	$tst_base = $_;
      }

      $exit_code = &tst_check($tst_base) || $exit_code;

      if ($tst_path ne "")
      {
	chdir($tst_curr_dir);
	print "cd $tst_curr_dir\n" if ($verbosity > 1);
      }
    }
    close (LST_FILE);
  }
  else
  {
    print (STDERR "Unknown extension of $_: Need extension lst or tst\n");
    $exit_code = 1;
  }
  if ($path ne "")
  {
    chdir($curr_dir);
    print "cd $curr_dir\n" if ($verbosity > 1);    
  }
}

# Und Tschuess
exit $exit_code;


