#!/usr/bin/perl

#################################################################
# $Id: regress.cmd,v 1.7 1998-04-30 07:40:10 obachman Exp $
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
  [-n]             -- do not ignore output from tst_ignore when diffing
  [-k]             -- keep result (*.res) files, do not zip original res file
  [-v num]         -- set verbosity to num (used range 0..3, default: 1)
  [-g]             -- generate result (*.res) files, only
  [file.lst]       -- read tst files from file.lst
  [file.tst]       -- test Singular script file.tst 
_EOM_
}

#################################################################
# 
# the default settings
#
$singularOptions = "-tqr12345678";
$ignore = "-I '^\/\/.*used time:' -I '^\/\/.*ignore:' -I '? error occurred in'";
$keep = "no";
$verbosity = 1;
$generate = "no";
$exit_code = 0;
$singular = "./Singular";
if ( (! (-e $singular)) || (! (-x $singular)))
{
  $singular = "../Singular";
}


#################################################################
# 
# used programs
#
$diff = "diff";
$gunzip = "gunzip";
$gzip = "gzip";
$rm = "rm";
$cp = "cp";
$uuencode = "uuencode";
$uudecode = "uudecode";

#################################################################
# 
# tst_check($root)
# 
sub mysystem
{
  local($call) = $_[0];
  local($exit_status);
  
  print "$call\n" if ($verbosity > 1);
  return (system $call);
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
  if ($generate ne "yes")
  {
    if ((-r "$root.res.gz.uu") && ! ( -z "$root.res.gz.uu"))
    {
      $exit_status 
	= $exit_status || &mysystem("$uudecode -o $root.res.gz $root.res.gz.uu; $gunzip -f $root.res.gz");
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
  if ($verbosity > 2)
  {
    $system_call = "$singular $singularOptions $root.tst | tee $root.new.res";
  }
  else
  {
    $system_call = "$singular $singularOptions $root.tst > $root.new.res";
  }
  # Go Singular, Go!
  $exit_status = & mysystem($system_call);
  
  #prepare diff call 
  & mysystem("$rm -rf $root.diff");
  if ($generate eq "yes")
  {
    $system_call = "$cp $root.new.res $root.res";
  }
  elsif ($verbosity > 0)
  {
    $system_call = "$diff $ignore $root.res $root.new.res | tee $root.diff";
  }
  else
  {
    $system_call = "$diff $ignore $root.res $root.new.res > $root.diff";
  }
    
  $exit_status = $exit_status || & mysystem($system_call);
  # there seems to be a bug here somewhere: even if diff reported
  # differenceses and exited with status != 0, then system still
  # returns exit status 0. Hence we manually need to find out whether
  # or not differences were reported: 
  # iff diff-file exists and has non-zero size
  $exit_status = $exit_status || (-e "$root.diff" && -s "$root.diff");

  # complain even if verbosity == 0
  if ($exit_status && $verbosity == 0)
  {
    print (STDERR "---!!! $root\n");
  }

  #time to clean up
  if ($keep eq "no" && $exit_status == 0 && $generate ne "yes")
  {
    & mysystem("$rm -rf $root.new.res $root.diff");
    if (-r "$root.res.gz.uu")
    {
      & mysystem("$rm -rf $root.res");
    }
    else
    {
      & mysystem("$gzip -f $root.res; $uuencode $root.res.gz $root.res.gz > $root.res.gz.uu; $rm -rf $root.res.gz");
    }
  } 
  elsif ($generate eq "yes")
  {
    & mysystem("$gzip -f $root.res; $uuencode $root.res.gz $root.res.gz > $root.res.gz.uu; $rm -rf $root.res.gz");
    if ($keep eq "yes")
    {
      & mysystem("mv $root.new.res $root.res");
    }
  }
  
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
  elsif (/^-n$/)
  {
    $ignore = "";
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
if ( ! (-e $singular))
{
  print (STDERR "Can not find $singular \n") && &Usage && die;
}
if (! (-x $singular))
{
  print (STDERR "Can not execute $singular \n") && Usage && die;
}

# now do the work
foreach (@ARGV)
{
  # get root and extension
  ($extension = $_) =~ s/.*\.([^\.]*)$/$1/;
  ($root = $_) =~ s/(.*)\.[^\.]*$/$1/;

  if ($extension eq "tst")
  {
    $exit_code = & tst_check($root) || $exit_code;
  }
  elsif ($extension eq "lst")
  {
    if (! open(LST_FILE, "<$_"))
    {
      print (STDERR "Can not open $_ for reading\n");
      $exit_code = 1;
      next;
    }
    while (<LST_FILE>)
    {
      if (/^;/)
      {
	print unless ($verbosity == 0);
	next;
      }
      next if (/^\s*$/);
      chop if (/\n$/);
      ($extension = $_) =~ s/.*\.([^\.]*)$/$1/;
      ($root = $_) =~ s/(.*)\.[^\.]*$/$1/;
      $exit_code = & tst_check($root) || $exit_code;
    }
    close (LST_FILE);
  }
  else
  {
    print (STDERR "Unknown extension of $_: Need extension lst or tst\n");
    $exit_code = 1;
  }
}

# Und Tschuess
exit $exit_code;


