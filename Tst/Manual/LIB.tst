LIB "tst.lib"; tst_init(); tst_ignore("// ** loaded");
  option(notloadLib); // show loading of libraries

                   // the names of the procedures of inout.lib
  LIB "inout.lib"; // are now known to Singular
tst_status(1);$
