#!/usr/bin/perl

#################################################################
# FILE:    regress.cmd
# PURPOSE: Script which runs regress test of Singular
# CREATED: 2/16/98
# AUTHOR:  obachman@mathematik.uni-kl.de

use Env;

# equivalent to command line options -CS, whihc is not supported any more
use open ':std', ':utf8';
use open IO => ':bytes';

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
  [-A num]          -- set timeout [in sec.] for executed Singular
  [-C name]         -- be TeamCity friendly, use "name" as a test-suite name
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
$gzip = "gzip -9";
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
  mysystem($call);

  open(CATCH_FILE, "<catch_$$");
  while (<CATCH_FILE>)
  {
    $output = $output.$_;
  }
  close(CATCH_FILE);
  mysystem("$rm -f catch_$$");
  return $output;
}

$WINNT = 1 if (mysystem("uname -a | $grep CYGWIN > /dev/null 2>&1") == 0);
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
# timeout for Singular execution (in seconds!)
$timeout  = 0;
# sed scripts which are applied to res files before they are diff'ed
$sed_scripts = "-e '/used time:/d' -e '/tst_ignore:/d' -e '/Id[:\$]/d' -e '/error occurred in/d' -e '/tst_status/d' -e'/init >>/d' -e 's/\\[[0-9]*:[0-9]*\\]//g'";
# default value (in %) above which differences are reported on -r
$report_val = 5;
# default value (in %) above which differences cause an error on -e
$error_val = 5;
# default value in 1/100 seconds, above which time differences are reported
$mintime_val = 100;
$hostname = mysystem_catch("hostname");
chop $hostname;

# flag indicating whether to produce TeamCity output ("" - no):
$teamcity = "";
# current argument: test file name?
$test_file = "";

# all previous test_file's:
%test_files = ();


#################################################################
#
# teamcity helpers:
#
sub myGetTCprop
{
  local($prop) = $_[0];
  return( mysystem_catch("cat \"\$TEAMCITY_BUILD_PROPERTIES_FILE\"|grep \"$prop=\"|sed \"s/$prop=//\"") );
}

sub tc_filter
{
  local($t) = $_[0];

  $t =~ s/\|/|\|/g;

  $t =~ s/\n/|n/g;
  $t =~ s/\r/|r/g;

  $t =~ s/\u0085/|x/g;
  $t =~ s/\u2028/|l/g;
  $t =~ s/\u2029/|p/g;

  ## \x{263A}

  $t =~ s/\'/|\'/g;
  $t =~ s/\[/|\[/g;
  $t =~ s/\]/|\]/g;
  return ($t);
}
sub putTCmsg
{
  if( length($teamcity) > 0 )
  {
    local($message) = $_[0];
    local($text) = $_[1];

    print( "\n##teamcity[$message $text]\n" );
  }
}
sub putTCmsgV
{
  local($message) = $_[0];
  local($unquotedValue) = tc_filter($_[1]);

  putTCmsg( $message, "\'$unquotedValue\'");
}
sub putTCmsgNV
{
  local($m) = $_[0];
  local($p) = $_[1];
  local($v) = tc_filter($_[2]);
  putTCmsg( $m, "$p=\'$v\'" );
}
sub putTCmsgNV2
{
  local($m) = $_[0];
  local($p) = $_[1];
  local($v) = tc_filter($_[2]);
  local($pp) = $_[3];
  local($vv) = tc_filter($_[4]);
  putTCmsg( $m, "$p='$v' $pp='$vv'" );
}

#################################################################
#
# teamcity routines:
#
sub blockOpened
{
  local($v) = $_[0];
  putTCmsgNV( "blockOpened", "name", $v);
}
sub blockClosed
{
  local($v) = $_[0];
  putTCmsgNV( "blockClosed", "name", $v);
}
sub tcError
{
  local($text) = tc_filter($_[0]);
  local($details) = tc_filter($_[1]);
  local($status) = tc_filter($_[2]);
  # The status attribute may take following values:
  # NORMAL, WARNING, FAILURE, ERROR.
  # The default value is NORMAL.
  # The errorDetails attribute is used only if status is ERROR, in other cases it is ignored.
  # This message fails the build in case its status is ERROR and "Fail build if an error message is logged by build runner" checkbox is checked on build configuration general settings page.

  ##teamcity[message text='<message text>' errorDetails='<error details>' status='<status value>']
  putTCmsg( "message", "text=\'$text\' errorDetails=\'$details\' status=\'$status\'");
}

sub tcFailure
{
  local($text) = tc_filter($_[0]);
  local($details) = tc_filter($_[1]);
  tcError( $text, $details, "FAILURE" );
}

sub tcLog
{
  local($text) = $_[0];
  putTCmsgNV2( "message", "text", $text, "status", "NORMAL");
}
sub tcWarn
{
  local($text) = $_[0];
  putTCmsgNV2( "message", "text", $text, "status", "WARNING");
}

sub testSuiteStarted
{
  local($v) = $_[0];
  putTCmsgNV( "testSuiteStarted", "name", $v);
}
sub testSuiteFinished
{
  local($v) = $_[0];
  putTCmsgNV( "testSuiteFinished", "name", $v);
}

$failed = 0;

sub testNameRemoveLeadingUnderscore
{
  # make NN/_abs.tst - > NN/abs.tst
  local($t) = $_[0];
  $t =~ s/^_([a-z0-9][^\/]*)$/\1/g;
  $t =~ s/[\/]_([a-z0-9][^\/]*)$/\/\1/g;
  return ($t);
}

sub testStarted
{
  local($v) = $_[0];
  $v = testNameRemoveLeadingUnderscore($v);
  putTCmsgNV2( "testStarted", "name", $v, "captureStandardOutput", "true");
  $failed = 0;
}
sub testFinished
{
  local($v) = $_[0];
  local($d) = $_[1];
  $v = testNameRemoveLeadingUnderscore($v);
  putTCmsgNV2( "testFinished", "name", $v, "duration", $d);
  $failed = 0;
}

sub testFailed
{
  local($n) = $_[0];
  local($m) = $_[1];

  $n = testNameRemoveLeadingUnderscore($n);

  if( !$failed )
  {
    putTCmsgNV2( "testFailed", "name", $n, "message", $m);
    $failed = 1;
  } else
  {
    tcFailure("Test: $n => $m", "");
  }
}
sub testFailed2
{
  local($n) = tc_filter($_[0]);
  local($m) = tc_filter($_[1]);
  local($t) = tc_filter($_[2]);

  $n = testNameRemoveLeadingUnderscore($n);

  if( !$failed )
  {
    putTCmsg( "testFailed", "name=\'$n\' message=\'$m\' details=\'$t\'");
    $failed = 1;
  } else
  {
    tcFailure("Test: $n => $m", $t);
  }

}
sub testFailedCMP
{
  local($name) = tc_filter($_[0]);
  local($msg) = tc_filter($_[1]);
  local($details) = tc_filter($_[2]);
  local($expected) = tc_filter($_[3]);
  local($actual) = tc_filter($_[4]);

  $name = testNameRemoveLeadingUnderscore($name);

  if( !$failed )
  {
    putTCmsg( "testFailed", "type=\'comparisonFailure\' name=\'$name\' message=\'$msg\' details=\'$details\' expected=\'$expected\' actual=\'$actual\'");
    $failed = 1;
  } else
  {
    tcFailure("Test: $name => $msg", "$detail\nExpected: $expected\nActual: $actual");
  }

}

##teamcity[testFailed type='comparisonFailure' name='test_file' message='failure_message' details='message and stack trace' expected='expected value' actual='actual value']
sub testIgnored
{
  local($n) = $_[0];
  local($m) = $_[1];

  $n = testNameRemoveLeadingUnderscore($n);

  putTCmsgNV2( "testIgnored", "name", $n, "message", $m);
}



#################################################################
#
# auxiallary routines
#

sub GetSingularVersionDate
{
  mysystem("$singular -t -v --execute=\"exit;\"> SingularVersionDate");
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
  mysystem("if [ -e /proc/cpuinfo ]; then cat /proc/cpuinfo >> SingularVersionDate; fi ");
  # mysystem("sysctl -a  >> SingularVersionDate"); # sysctl is not a user cmd.
  mysystem("uname -a >> SingularVersionDate");
  mysystem("if [ -e /proc/meminfo ]; then cat /proc/meminfo >> SingularVersionDate; fi ");
  # mysystem("free -h >> SingularVersionDate"); # nobody supports free -h
}

sub Diff
{
  local($root) = $_[0];
  local($exit_status);

  # prepare the result files:
  mysystem("$cat \"$root.res\" | $tr -d '\\013' | $sed $sed_scripts > \"$root.res.cleaned\"");
  mysystem("$cat \"$root.new.res\" | $tr -d '\\013' | $sed $sed_scripts > \"$root.new.res.cleaned\"");

  # doo the diff call
  $exit_status = mysystem("$diff -w -b \"$root.res.cleaned\" \"$root.new.res.cleaned\" > \"$root.diff\" 2>&1");


  # clean up time
  mysystem("$rm -f \"$root.res.cleaned\" \"$root.new.res.cleaned\"");

  # there seems to be a bug here somewhere: even if diff reported
  # differenceses and exited with status != 0, then system still
  # returns exit status 0. Hence we manually need to find out whether
  # or not differences were reported:
  # iff diff-file exists and has non-zero size
  $exit_status = $exit_status || (-e "$root.diff" && -s "$root.diff");

  if( $exit_status && (length($teamcity) > 0) )
  {
    local($details) = mysystem_catch("$cat \"$root.diff\"");
    local($expected) = mysystem_catch("$cat \"$root.res\"");
    local($actual) = mysystem_catch("$cat \"$root.new.res\"");
    testFailedCMP($test_file, "Differences in res files", $details, $expected, $actual )
  }

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
      mysystem("$rm \"$root.tmp.stat\"");
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
  mysystem("$mv -f \"$root.tmp.stat\" \"$root.stat\"");
  mysystem("$rm -f \"$root.new.stat\" \"$root.stat.sdiff\"") unless $keep eq "yes";
  return ;
}

sub tst_check
{
  local($root) = $_[0];
  local($system_call, $exit_status, $ignore_pattern, $error_cause);

  local($my_test_file) = testNameRemoveLeadingUnderscore($test_file);

  if (! ($my_test_file eq $test_file))
  {
    tcLog("The test '$test_file' will be reported to TeamCity as '$my_test_file' as it was _renamed :-/");
  }

  if( exists($test_files{$test_file}) && (length($teamcity) > 0) )
  {
     tcWarn("The test '$test_file' have been alreeady tests (with result: $test_files{$test_file})... skipping!");
     return ($test_files{$test_file})
  }

  $total_checks++;

  # check for existence/readablity of tst and res file
  if (! (-r "$root.tst"))
  {
    print "--- $root " unless ($verbosity == 0);
    print (STDERR "Can not read $root.tst\n");
    testIgnored($test_file, "Can not read $root.tst");
    $test_files{$test_file} = 1;
    return (1);
  }

  # generate $root.res
  if ($generate ne "yes" && ! defined($mtrack) && !defined($timings_only))
  {
    if ((-r "$root.res.gz.uu") && ! ( -z "$root.res.gz.uu"))
    {
      $exit_status = mysystem("$uudecode \"$root.res.gz.uu\" > /dev/null 2>&1; $gunzip -f \"$root.res.gz\"");
      if ($exit_status)
      {
        print "--- $root " unless ($verbosity == 0);
        print (STDERR "Can not decode $root.res.gz.uu\n");
        testIgnored($test_file, "Can not decode $root.res.gz.uu");
        $test_files{$test_file} = $exit_status;
        return ($exit_status);
      }
    }
    elsif (! (-r "$root.res") || ( -z "$root.res"))
    {
      print "--- $root " unless ($verbosity == 0);
      print (STDERR "Can not read $root.res[.gz.uu]\n");
      testIgnored($test_file, "Can not read $root.res[.gz.uu]");
      $test_files{$test_file} = 1;
      return (1);
    }
  }

  testStarted($test_file);
  print "--- $root " unless ($verbosity == 0);

  my $resfile = "\"$root.new.res\"";
  $resfile = "\"$root.mtrack.res\"" if (defined($mtrack));
  my $statfile = "$root.new.stat";
  mysystem("$rm -f \"$statfile\"");

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
  $exit_status = mysystem($system_call);
  my ($user_t,$system_t,$cuser_t2,$csystem_t2) = times;
  $cuser_t = $cuser_t2 - $cuser_t;
  $csystem_t = $csystem_t2 - $csystem_t;

  tcLog("Test: $test_file, user time: $cuser_t, system time: $csystem_t" );

  if ($exit_status != 0)
  {
    $error_cause = "Singular call exited with status ($exit_status) != 0";

    if( length($teamcity) > 0 )
    {
      ### TODO: add a way to detect timeout!!!
      if( $exit_status == 142 ) # perl sig alarm exit code? NOPE :(((
      {
        local($details) = mysystem_catch("$cat \"$resfile\"");
	testFailed2($test_file, "Exit on timeout ($timeout sec)!", $details);
      }
    }
  }
  else
  {
    # check for Segment fault in res file
    $exit_status = ! (mysystem("$grep \"Segment fault\" $resfile > /dev/null 2>&1"));

    if ($exit_status)
    {
      $error_cause = "Segment fault";
      local($details) = mysystem_catch("$cat \"$resfile\"");
      testFailed2($test_file, $error_cause, $details);
    }
    elsif (! defined($mtrack) && !defined($timings_only))
    {
      mysystem("$rm -f \"$root.diff\"");
      if ($generate eq "yes")
      {
        mysystem("$cp $resfile \"$root.res\"");
      }
      else
      {
        # call Diff
        $exit_status = Diff($root);
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
          mysystem("$rm -f \"$root.diff\"");
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
      ($exit_status, $error_cause) = tst_status_check($root);
    }
    else
    {
      print "Warning: no file $statfile\n";
      tcWarn("Warning: no file $statfile");
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

    if( length($teamcity) > 0 )
    {
      local($details) = mysystem_catch("$cat \"$resfile\"");
      testFailed2($test_file, $error_cause, $details);
    }
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
	  mysystem("$gzip -cf \"$root.res\" | $uuencode \"$root.res.gz\" > \"$root.res.gz.uu\"");
	}
	else
	{
	  # uuencode is broken under windows
	  print "Warning: Can not generate $root.res.gz.uu under Windows\n";
	}
      }
      elsif (%merge)
      {
	($exit_status, $error_cause) = tst_status_merge($root);

	print (STDERR "Warning: Merge Problems: $error_cause\n")
	  if ($verbosity > 0 && $exit_status);
      }
    }
    if ($keep ne "yes")
    {
      mysystem("$rm -f tst_status.out $resfile \"$root.res\" \"$root.diff\" \"$root.new.stat\"");
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

  mysystem("mv gmon.out \"gmon.$root.out\"") if (-e "gmon.out");

  testFinished($test_file, $cuser_t + $csystem_t);

  $test_files{$test_file} = $exit_status;
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
  elsif (/^-A/)
  {
    $timeout = shift;
  }
  elsif (/^-C$/)
  {
    $teamcity = shift;
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

sub ViewFile
{
  local($f) = $_[0];

  local($ff) = myGetTCprop($f);
  local($b) = "$f: " . $ff;

  blockOpened ($b);
  mysystem("cat " . $ff);
  blockClosed ($b);
}


# if( length($teamcity) > 0 )
# {
#  #  tcLog("|Hi|\r I\'m [Alex]|\nHow are You?|");
#
#  blockOpened ("init");
#
# #  print ("TEAMCITY_BUILD_PROPERTIES_FILE: $ENV{TEAMCITY_BUILD_PROPERTIES_FILE}" );
#
#  tcLog("TEAMCITY_BUILD_PROPERTIES_FILE: $ENV{TEAMCITY_BUILD_PROPERTIES_FILE}");
#
#  if ( length("$ENV{TEAMCITY_BUILD_PROPERTIES_FILE}") > 0 )
#  {
#    print( "teamcity.tests.runRiskGroupTestsFirst: " . myGetTCprop("teamcity.tests.runRiskGroupTestsFirst") . "\n" );
#
#    ViewFile("teamcity.tests.recentlyFailedTests.file");
#    ViewFile("teamcity.build.changedFiles.file");
#    ViewFile("teamcity.build.properties.file");
#    ViewFile("teamcity.configuration.properties.file");
#    ViewFile("teamcity.runner.properties.file");
#  }
#
#  blockClosed ("init");
# }

if ($timeout > 0)
{
  $singular = "PERL_SIGNALS=unsafe perl -e 'alarm($timeout);exec(\@ARGV);' $singular";
  tcLog ("Set exec timeout to $timeout sec.\n");
  # die;
}

# testSuiteStarted($teamcity);

# now do the work
foreach (@ARGV)
{
  if( /^([^:]*): *(.*)$/ )
  {
    $_=$2;
  }

  if ( /^\s*([^ ].*)$/ )
  {
    $_ = $1;
  }

  if ( /^\.\/(.*)$/ )
  {
    $_ = $1;
  }

  $test_file = $_;

  tcLog("test_file: $test_file");

  if ( /^(.*)\.([^\.\/]*)$/ )
  {
    $_ = $1;
    $extension = $2;
  } else
  {
#    print ("Wrong input: [$_] has no extension!");
#    tcWarn("Wrong input: [$_] has no extension!");
    next;
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

  tcLog("path: $path, base: $base, extension: $extension");

  $file = "$base.$extension";
  chop ($tst_curr_dir = `pwd`);

  if ($extension eq "tst")
  {
    $exit_code = tst_check($base) || $exit_code;
  }
  elsif ($extension eq "lst")
  {
    if (! open(LST_FILE, "<$file"))
    {
      print (STDERR "Can not open $path/$file for reading\n");
      $exit_code = 1;
      testIgnored($test_file, "Can not open $path/$file for reading");
      next;
    }

    local ($b) = $test_file;
    blockOpened ($b);

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
  if( /^([^:]*): *(.*)$/ )
  {
    $_=$2;
  }

  if ( /^\s*([^\s].*)$/ )
  {
    $_ = $1;
  }

  if ( /^\.\/(.*)$/ )
  {
    $_ = $1;
  }

      next if (/^\s*$/); #ignore whitespaced lines
      chop if (/\n$/);   #chop of \n

      tcLog("path: $path, test_file: $_, file: $file");

      if (length($path) > 0)
      {
        $test_file = "$path/$_";
      }
      else
      {
        $test_file = $_;
      }

      $test_file =~ s/^[ ]*\.\///;


      $_ = $1 if (/^(.*)\.([^\.\/]*)$/ ); # chop of extension (.tst!!!?)
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

      tcLog("tst_path: $tst_path, tst_base: $tst_base");

      my $this_exit_code = tst_check($tst_base);

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
      unless ($verbosity < 2);

    tcLog( sprintf("list '$base' Summary: Checks:$lst_checks Failed:%d Time:%.2f", $lst_checks - $lst_checks_pass, $lst_used_time) );
    blockClosed ($b);
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

tcLog( sprintf("Global Summary: Checks:$total_checks Failed:%d Time:%.2f", $total_checks - $total_checks_pass, $total_used_time)) ;

if( length($teamcity) > 0 )
{
#  testSuiteFinished($teamcity);

#  blockOpened ("init");

#  print ("TEAMCITY_BUILD_PROPERTIES_FILE: $ENV{TEAMCITY_BUILD_PROPERTIES_FILE}" );

#  blockClosed ("init");



}


# Und Tschuess
if ($exit_code != 0 && ($exit_code % 256) == 0)
{
  $exit_code = 1;
}
exit $exit_code;


