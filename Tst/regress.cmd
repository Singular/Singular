#!/usr/bin/perl

#################################################################
# $Id$
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
  [-s <Singular>]   -- use <Singular> as executable to test
  [-h]              -- print out help and exit
  [-k]              -- keep all intermediate files
  [-v num]          -- set verbosity to num (used range 0..4, default: 2)
  [-g]              -- generate result (*.res.gz.uu) files, only
  [-r [crit%[val]]] -- report if status differences [of crit] > val (in %)
  [-c regexp]       -- when comparing results, version must match this regexp
  [-e [crit%[val]]] -- throw error if status difference [of crit] > val (in %)
  [-a [crit]]       -- add status results [of crit] to result file
  [-m]              -- add status result for current version to result file
  [-t]              -- compute and call system("mtrack", 1) at the end, no diffs
  [-tt max]         -- compute and call system("mtrack", max) at the end
  [-T]              -- simply compute and determine timmings, no diffs
  [file.lst]        -- read tst files from file.lst
  [file.tst]        -- test Singular script file.tst
_EOM_
}

#################################################################
#
# used programs
#
$sh="/bin/sh";
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
$grep = "grep";

sub mysystem
{
  local($call) = $_[0];
  local($exit_status);

  $call =~ s/"/\\"/g;
  $call = "$sh -c \"$call\"";
  print "$call\n" if ($verbosity > 2);
  return (system $call);
}

sub mysystem_catch
{
  local($call) = $_[0];
  local($output) = "";

  $call = "$call > catch_$$";
  & mysystem($call);

  open(CATCH_FILE, "<catch_$$");
  while (<CATCH_FILE>)
  {
    $output = $output.$_;
  }
  close(CATCH_FILE);
  & mysystem("$rm -f catch_$$");
  return $output;
}

$WINNT = 1 if (&mysystem("uname -a | $grep CYGWIN > /dev/null 2>&1") == 0);
$uuencode = "uuencode";
$uudecode = "uudecode";

#
# flush stdout and stderr after every write
#
select(STDERR);
$| = 1;
select(STDOUT);
$| = 1;

#################################################################
#
# the default settings
#
$singularOptions = "--ticks-per-sec=100 -teqsr12345678 --no-rc";
# for testing under Windows, remove "e" option above and compile
# res files on Linux, then copy to Windows and compare. Otherwise
# you have problems with diff on Win. Just uncomment the next line:
# $singularOptions = "--ticks-per-sec=100 -tqsr12345678 --no-rc";

$keep = "no";
$verbosity = 2;
$generate = "no";
$exit_code = 0;
chop($curr_dir=`pwd`);
# singular -- use the one in curr directory or the one found above
$ext=".exe" if ($WINNT);
$singular = "$curr_dir/Singular$ext";
if ( (! (-e $singular)) || (! (-x $singular)))
{
  $singular = $curr_dir."/../Singular$ext";
}
# sed scripts which are applied to res files before they are diff'ed
$sed_scripts = "-e '/used time:/d' -e '/tst_ignore:/d' -e '/Id[:\$]/d' -e '/error occurred in/d' -e '/tst_status/d' -e'/init >>/d' -e 's/\\[[0-9]*:[0-9]*\\]//g'";
# default value (in %) above which differences are reported on -r
$report_val = 5;
# default value (in %) above which differences cause an error on -e
$error_val = 5;
# default value in 1/100 seconds, above which time differences are reported
$mintime_val = 100;
$hostname = &mysystem_catch("hostname");
chop $hostname;

#################################################################
#
# auxiallary routines
#

sub GetSingularVersionDate
{
  &mysystem("$singular -t -v --execute=\"exit;\"> SingularVersionDate");
  open(FD, "<SingularVersionDate");
  while (<FD>)
  {
    $singular_uname = (/for\s+([^\s]*)\s+/ ? $1 : "uname");
    $singular_version = (/version\s+([^\s]*)\s+/ ? $1 : "0-0-0");
    $singular_date = (/\((.*)\)/ ? $1 : "1970010100");
    $this_time = time;
    last;
  }
  close(FD);
}

sub Set_withMP
{
  if (! $withMP)
  {
    $withMP = "no";
    open(MP_TEST, ">MPTest");
    print(MP_TEST "system(\"with\", \"MP\"); \$");
    close(MP_TEST);
    &mysystem("$singular -qt MPTest > withMPtest");
    if (open(MP_TEST, "<withMPtest"))
    {
      $_ = <MP_TEST>;
      $withMP = "yes" if (/^1/);
      close(MP_TEST);
    }
    &mysystem("$rm -f withMPtest MPTest");
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
  &mysystem("$cat \"$root.res\" | $tr -d '\\013' | $sed $sed_scripts > \"$root.res.cleaned\"");
  &mysystem("$cat \"$root.new.res\" | $tr -d '\\013' | $sed $sed_scripts > \"$root.new.res.cleaned\"");

  # doo the diff call
  $exit_status = &mysystem("$diff -w -b \"$root.res.cleaned\" \"$root.new.res.cleaned\" > \"$root.diff\" 2>&1");

  # clean up time
  &mysystem("$rm -f \"$root.res.cleaned\" \"$root.new.res.cleaned\"");

  # there seems to be a bug here somewhere: even if diff reported
  # differenceses and exited with status != 0, then system still
  # returns exit status 0. Hence we manually need to find out whether
  # or not differences were reported:
  # iff diff-file exists and has non-zero size
  $exit_status = $exit_status || (-e "$root.diff" && -s "$root.diff");

  return($exit_status);
}

sub tst_status_check
{
  local($root) = $_[0];
  local($line,$new_line,$prefix,$crit,$res,$new_res);
  local($res_diff,$res_diff_pc,$res_diff_line);
  my($exit_status, $reported) = (0, 0);
  local($error_cause) = "";

  open(RES_FILE, "<$root.stat") ||
    return (1, "Can not open $root.stat \n");
  open(NEW_RES_FILE, "<$root.new.stat") ||
    return (1, "Can not open $root.new.stat \n");
  open(STATUS_DIFF_FILE, ">$root.stat.sdiff") ||
    return (1, "Can not open $root.stat.sdiff \n");

  while (1)
  {
    while ($new_line = <NEW_RES_FILE>)
    {
      last if $new_line =~ /(\d+) >> (\w+) ::.*$hostname:(\d+)/ && $checks{$2};
    }
    last unless $new_line =~ /(\d+) >> (\w+) ::.*$hostname:(\d+)/;
    $prefix = $1;
    $crit = $2;
    $new_res = $3;
    next unless $new_res > $mintime_val;
    
    while ($line = <RES_FILE>)
    {
      last if $line =~ /$prefix >> $crit ::.*?$hostname:(\d+)/;
    }
    last unless $line =~ /$prefix >> $crit ::.*?$hostname:(\d+)/;
    my $res_version;
    $res = 0;

    # search for smallest
    while ($line =~ /([^\s]*)$hostname:(\d+)/g)
    {
      my $this_res = $2;
      my $this_res_version = $1;
      if ((!$res || $this_res <= $res) && (!$status_check_regexp  || $this_res_version =~ /$status_check_regexp/))
      {
	$res = $this_res;
	$res_version = $this_res_version;
      }
    }
    next unless $res;
    $res_diff = $new_res - $res;
    $res_diff_pc = int((($new_res / $res) - 1)*100);
    $res_diff_line = 
      "$prefix >> $crit :: new:$new_res old:$res_version$res diff:$res_diff %:$res_diff_pc\n";
    print STATUS_DIFF_FILE $res_diff_line;
    
    if ((defined($error{$crit}) &&  $error{$crit}<abs($res_diff_pc))
	||
	(defined($report{$crit}) && $report{$crit}<abs($res_diff_pc)))
    {
      print "$prefix >> $crit :: new:$new_res old:$res diff:$res_diff %:$res_diff_pc\n"
	if ($verbosity > 0);
    }
    
    if ($exit_status == 0)
    {
      $exit_status = (defined($error{$crit})
		      && $error{$crit} < abs($res_diff_pc));
      $error_cause = "Status error for $crit at $prefix\n"
	if ($exit_status);
    }
  }
  close(RES_FILE);
  close(NEW_RES_FILE);
  close(STATUS_DIFF_FILE);
  return ($exit_status, $error_cause);
}

sub tst_status_merge
{
  local($root) = $_[0];
  local($line, $new_line, $crit, $res);

  GetSingularVersionDate()
    unless $singular_version;

  if (! -e "$root.stat")
  {
    open(RES_FILE, ">$root.stat") ||
      return (1, "Can not open $root.stat \n");
    open(NEW_RES_FILE, "<$root.new.stat") ||
      return (1, "Can not open $root.new.stat \n");

    while (<NEW_RES_FILE>)
    {
      if (/(\d+) >> (\w+) :: /)
      {
	s/$hostname:(\d+)/$this_time:$singular_date:$singular_version:$singular_uname:$hostname:$1/g;
	print RES_FILE $_;
      }
    }
    close(RES_FILE);
    close(NEW_RES_FILE);
    return;
  }

  open(RES_FILE, "<$root.stat") ||
    return (1, "Can not open $root.stat \n");
  open(NEW_RES_FILE, "<$root.new.stat") ||
    return (1, "Can not open $root.new.stat \n");
  open(TEMP_FILE, ">$root.tmp.stat") ||
    return (1, "Can not open $root.tmp.stat \n");

  while (1)
  {
    while (($new_line = <NEW_RES_FILE>) && $new_line !~ /(\d+) >> (\w+) ::/){}
    last unless $new_line =~ /(\d+) >> (\w+) ::.*$hostname:(\d+)/;
    my $prefix = $1;
    my $crit = $2;
    my $new_res = "$this_time:$singular_date:$singular_version:$singular_uname:$hostname:$3";
    while (($line = <RES_FILE>) && $line !~ /$prefix >> $crit ::/){}
    unless ($line)
    {
      close(RES_FILE);
      close(NEW_RES_FILE);
      close(TEMP_FILE);
      &mysystem("$rm \"$root.tmp.stat\"");
      return (1, "Can not find '$prefix >> $crit' in $root.stat\n");
    }
    if ($merge_version)
    {
      $line =~ s/[^ ]*:$singular_version:$singular_uname:$hostname:\d+//g;
      chop $line;
      $line .= " $new_res\n";
    }
    else
    {
      chop $line;
      $line .= " $new_res\n";
    }
    print TEMP_FILE $line;
  }
  
  close(RES_FILE);
  close(NEW_RES_FILE);
  close(TEMP_FILE);
  &mysystem("$mv -f \"$root.tmp.stat\" \"$root.stat\"");
  &mysystem("$rm -f \"$root.new.stat\" \"$root.stat.sdiff\"") unless $keep eq "yes";
  return ;
}

sub tst_check
{
  local($root) = $_[0];
  local($system_call, $exit_status, $ignore_pattern, $error_cause);

  print "--- $root " unless ($verbosity == 0);
  $total_checks++;
  
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
  if ($generate ne "yes" && ! defined($mtrack) && !defined($timings_only))
  {
    if ((-r "$root.res.gz.uu") && ! ( -z "$root.res.gz.uu"))
    {
      $exit_status = &mysystem("$uudecode \"$root.res.gz.uu\" > /dev/null 2>&1; $gunzip -f \"$root.res.gz\"");
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

  my $resfile = "\"$root.new.res\"";
  $resfile = "\"$root.mtrack.res\"" if (defined($mtrack));
  my $statfile = "$root.new.stat";
  &mysystem("$rm -f \"$statfile\"");
  if (defined($mtrack))
  {
    $system_call = "$cat \"$root.tst\" | sed -e 's/\\\\\$/LIB \"general.lib\"; killall(); killall(\"proc\");kill killall;system(\"mtrack\", \"$root.mtrack.unused\", $mtrack); \\\$/' | $singular $singularOptions ";
    $system_call .= ($verbosity > 3 ? " | $tee " : " > ");
    $system_call .= "\"$root.mtrack.res\"";
    $system_call .= " 2>&1 " if ($verbosity <= 3);
  }
  else
  {
    
    # prepare Singular run
    if ($verbosity > 3 && !$WINNT)
    {
      $system_call = "$cat \"$root.tst\" | $singular --execute 'string tst_status_file=\"$statfile\";' $singularOptions | $tee $resfile";
    }
    else
    {
      $system_call = "$cat \"$root.tst\" | $singular --execute 'string tst_status_file=\"$statfile\";' $singularOptions > $resfile 2>&1";
    }
  }
  # Go Singular, Go!
  
  my ($user_t,$system_t,$cuser_t,$csystem_t) = times;
  $exit_status = &mysystem($system_call);
  my ($user_t,$system_t,$cuser_t2,$csystem_t2) = times;
  $cuser_t = $cuser_t2 - $cuser_t;
  $csystem_t = $csystem_t2 - $csystem_t;
  if ($exit_status != 0)
  {
    $error_cause = "Singular call exited with status != 0";
  }
  else
  {
    # check for Segment fault in res file
    $exit_status = ! (&mysystem("$grep \"Segment fault\" $resfile > /dev/null 2>&1"));

    if ($exit_status)
    {
      $error_cause = "Segment fault";
    }
    elsif (! defined($mtrack) && !defined($timings_only))
    {
      &mysystem("$rm -f \"$root.diff\"");
      if ($generate eq "yes")
      {
        &mysystem("$cp $resfile \"$root.res\"");
      }
      else
      {
        # call Diff
        $exit_status = &Diff($root);
        if ($exit_status)
        {
	  unless ($verbosity == 0)
	  {
	    print "\n";
	    mysystem("$cat \"$root.diff\"");
	  }
          $error_cause = "Differences in res files";
        }
        else
        {
          &mysystem("$rm -f \"$root.diff\"");
        }
      }
    }
  }



  mysystem("mv tst_status.out \"$statfile\"")
    if (! -e $statfile && -e "tst_status.out");

  if (%checks && ! $exit_status && $generate ne "yes" && ! defined($mtrack))
  {
    if (-e "$statfile")
    {
      # do status checks
      ($exit_status, $error_cause) = & tst_status_check($root);
    }
    else
    {
      print "Warning: no file $statfile\n";
    }
  }


  # complain even if verbosity == 0
  if ($exit_status)
  {
    if (! -e "$root.diff")
    {
      open (DIFF_FILE, ">$root.diff");
      print DIFF_FILE "!!! $root : $error_cause\n";
      print "\n";
    }
    print STDERR "!!! $root : $error_cause\n";
  }
  else
  {
    unless (defined($mtrack))
    {
      #clean up
      if ($generate eq "yes")
      {
	mysystem("$rm -f \"$root.stat\"") unless %merge;
	($exit_status, $error_cause) = tst_status_merge($root);
	if (! $WINNT)
	{
	  &mysystem("$gzip -cf \"$root.res\" | $uuencode \"$root.res.gz\" > \"$root.res.gz.uu\"");
	}
	else
	{
	  # uuencode is broken under windows
	  print "Warning: Can not generate $root.res.gz.uu under Windows\n";
	}
      }
      elsif (%merge)
      {
	($exit_status, $error_cause) = & tst_status_merge($root);
	  
	print (STDERR "Warning: Merge Problems: $error_cause\n")
	  if ($verbosity > 0 && $exit_status);
      }
    }
    if ($keep ne "yes")
    {
      &mysystem("$rm -f tst_status.out $resfile \"$root.res\" \"$root.diff\" \"$root.new.stat\"");
    }
  }
  # und tschuess
  unless ($verbosity == 0 || $exit_status)
  {
    if ($verbosity > 1 || $timings_only)
    {
      my $used_time = $cuser_t + $csystem_t;
      $total_used_time += $used_time;
      $lst_used_time += $used_time;
      print " " x (23 - length($root));
      printf("%.2f", $used_time);
    }
    print " \n";
  }
  $total_checks_pass++ unless $exit_status;

  mysystem("mv gmon.out \"gmon.$root.out\"")
    if (-e "gmon.out");

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
  elsif (/^-tt/)
  {
    $mtrack = shift;
  }
  elsif(/^-t$/)
  {
    $mtrack = 1;
  }
  elsif (/^-T/)
  {
    $timings_only = 1;
  }
  elsif(/^-r$/)
  {
    $crit = "all";
    $val = $report_val;
    if ($ARGV[0] =~ /.*%.*/)
    {
      ($crit, $val) = split(/%/, shift);
    }
    elsif ($ARGV[0] &&
           $ARGV[0] !~ /^-/ && $ARGV[0] !~ /.*\.tst/ && $ARGV[0] !~ /.*\.lst/)
    {
      $crit = shift;
    }
    if ($crit eq "all")
    {
      $report{"tst_memory_0"} = $val;
      $report{"tst_memory_1"} = $val;
      $report{"tst_memory_2"} = $val;
      $report{"tst_timer"} = $val;
      $report{"tst_timer_1"} = $val;
      $checks{"tst_memory_0"} = 1;
      $checks{"tst_memory_1"} = 1;
      $checks{"tst_memory_2"} =  1;
      $checks{"tst_timer"} =  1;
      $checks{"tst_timer_1"} =  1;
    }
    else
    {
      $report{$crit} = $val;
      $checks{$crit} = 1;
    }
  }
  elsif(/^-e$/)
  {
    $crit = "all";
    $val = $error_val;
    if ($ARGV[0] =~ /.*%.*/)
    {
      ($crit, $val) = split(/%/, shift);
    }
    elsif ($ARGV[0] &&
            $ARGV[0] !~ /^-/ && $ARGV[0] !~ /.*\.tst/ && $ARGV[0] !~ /.*\.lst/)
    {
      $crit = shift;
    }
    if ($crit eq "all")
    {
      $error{"tst_memory_0"} = $val;
      $error{"tst_memory_1"} = $val;
      $error{"tst_memory_2"} = $val;
      $error{"tst_timer"} = $val;
      $error{"tst_timer_1"} = $val;
      $checks{"tst_memory_0"} = 1;
      $checks{"tst_memory_1"} = 1;
      $checks{"tst_memory_2"} =  1;
      $checks{"tst_timer"} =  1;
      $checks{"tst_timer_1"} =  1;
    }
    else
    {
      $error{$crit} = $val;
      $checks{$crit} = 1;
    }
  }
  elsif(/^-a/ || /^-m/)
  {
    $merge_version = 1 if /^-m/;
    if ($ARGV[0] &&
        $ARGV[0] !~ /^-/ && $ARGV[0] !~ /.*\.tst/ && $ARGV[0] !~ /.*\.lst/)
    {
      $crit = shift;
      $merge{$crit} = 1;
    }
    else
    {
      $merge{"tst_memory_0"} = 1;
      $merge{"tst_memory_1"} = 1;
      $merge{"tst_memory_2"} =  1;
      $merge{"tst_timer"} =  1;
      $merge{"tst_timer_1"} =  1;
    }
  }
  elsif (/^-c/)
  {
    $status_check_regexp = shift;
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
$singular = "$curr_dir/$singular" unless ($singular =~ /^\/.*/);

if ( ! (-e $singular))
{
  $singular = "$singular$ext"   if ($WINNT && $singular !~ /.*$ext$/);
}

if ( ! (-e $singular))
{
  print (STDERR "Can not find $singular \n") && &Usage && die;
}

if (! (-x $singular) && (! WINNT))
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
    print "cd $path\n" if ($verbosity > 2);
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
    $lst_used_time = 0;
    $lst_checks = 0;
    $lst_checks_pass = 0;
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
        print "cd $tst_path\n" if ($verbosity > 2);
      }
      else
      {
        $tst_path = "";
        $tst_base = $_;
      }
      $tst_base =~ s/^\s*//;
      $tst_base =~ s/(.*?)\s+.*/$1/;
      $lst_checks++;
      my $this_exit_code = &tst_check($tst_base);
      $lst_checks_pass++ unless $this_exit_code;
      $exit_code = $this_exit_code || $exit_code;

      if ($tst_path ne "")
      {
        chdir($tst_curr_dir);
        print "cd $tst_curr_dir\n" if ($verbosity > 2);
      }
    }
    close (LST_FILE);
    printf("$base Summary: Checks:$lst_checks Failed:%d Time:%.2f\n", $lst_checks - $lst_checks_pass, $lst_used_time) 
      unless ($verbosity < 2)
  }
  else
  {
    print (STDERR "Unknown extension of $_: Need extension lst or tst\n");
    $exit_code = 1;
  }
  if ($path ne "")
  {
    chdir($curr_dir);
    print "cd $curr_dir\n" if ($verbosity > 2);
  }
}

unless ($verbosity < 2 || $lst_checks == $total_checks)
{
  printf("Summary: Checks:$total_checks Failed:%d Time:%.2f\n", $total_checks - $total_checks_pass, $total_used_time);
}

# Und Tschuess
exit $exit_code;


