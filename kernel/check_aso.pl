#!/usr/local/bin/perl
###########################################################################
# $Id: check_aso.pl,v 1.1.1.1 2003-10-06 12:15:50 Singular Exp $

###########################################################################
##
## FILE: check_aso.pl
## PURPOSE: checks source files for Alloc(sizeof(..)) statements and
##          fixes them, if requested
## AUTHOR: obachman (10/99)
##

$Usage = <<EOT;
$0 [-fh] file(s)
Checks source files for Alloc(sizeof(..)) statements and 
fixes them, if -f
EOT

# parse command-line options  
while  (@ARGV && $ARGV[0] =~ /^-/) 
{
  $_ = shift(@ARGV);
  if (/^-h/)
  {
    print STDOUT $Usage;
    exit(0);
  }
# $fix = 1 if /^-f/;
}


# process file 
while (<>)
{
  # Check for Alloc(sizeof(..)), Free(..,sizeof(..))
  if (/(^|[^\w])(Alloc|Alloc0|AllocAligned|AllocAligned0)\s*\(\s*sizeof\s*\(\s*([^\)]*)\s*\)\s*\)/ 
      ||
      /(^|[^\w])(Free|FreeAligned)\s*\(.*,\s*sizeof\s*\(\s*([^\)]*)\s*\)\s*\)/)
  {
    $what = $3;
    $alloc = $2;
    $what =~ s/^\s*//;
    $what =~ s/\s*$//;
    if ($what !~ /^\w[\w\d]*$/)
    {
      print STDERR "Warning: Argument $what to $alloc does not match \w[\w\d]* at $ARGV:$.\n";
    }
    else
    {
      # error if inside sizeof(..) is weird
      print STDERR "Warning: $alloc(sizeof($what)) found at $ARGV:$.";
      if ($fix)
      {
	s/sizeof\s*\(\s*$what\s*\)/$what/; 
	$what = $alloc."SizeOf";
	s/$alloc/$what/;
	$fixed = 1;
	print " (fixed)\n";
      }
      else
      {
	print "\nRun '$0 -f $ARGV' to fix\n";
      }
    }
  }
  $content .= $_ if $fix;
} 
continue
{
  if (eof)
  {
    close ARGV;
    if ($fix && $fixed)
    {
      print STDERR "Creating $ARGV.bak and fixing $ARGV\n";
      system("cp $ARGV $ARGV.bak");
      open(ARGV, ">$ARGV");
      print ARGV $content;
      close(ARGV);
      $fixed = 0;
    }
  }
}

