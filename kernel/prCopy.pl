#!/usr/bin/perl
# generate prCopy.inc

@defines = ("PR_DELETE_SRC",  "Move", "Copy",
            "PR_RING_EQUAL",   "REqual", "NoREqual",
            "PR_NUMBER_SIMPLE", "NSimple", "NoNSimple", 
            "PR_NO_SORT", "NoSort", "Sort");

@def = ("Move", "REqual", "NSimple", "NoSort");
@notdef = ("Copy", "NoREqaul", "NoNSimple", "Sort");

sub Generate
{
  my ($prefix, @args) = @_;

  if (@args)
  {
    my $name = shift @args;
    my $yes = shift @args;
    my $no = shift @args;
    print "#undef $name\n";
    print "#define $name 0\n";
    Generate($prefix."_".$no, @args);
    print "#undef $name\n";
    print "#define $name 1\n";
    Generate($prefix."_".$yes, @args);
  }
  else
  {
    print "#undef PR_NAME\n";
    print "#define PR_NAME $prefix\n";
    print "#include <kernel/prCopyMacros.h>\n";
    print "#include <kernel/prCopyTemplate.cc>\n";
  }
  print "\n";
}

Generate("pr", @defines);
