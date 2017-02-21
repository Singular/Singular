$library = "python_module.so";
$version = "(1.2,2002/07/03)";
$category = <<EOT;
tests
EOT
$info = <<EOT;
LIBRARY: kernel.lib  PROCEDURES OF GENERAL TYPE WRITEN IN C python(input); eval a string  in python
EOT
push(@procs, "python");
$help{"python"} = <<EOT;
No help available for this command
EOT
$example{"python"} = <<EOT;

  python("print 1+1");

EOT
$chksum{"python"} = 0;
1;