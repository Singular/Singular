LIB "tst.lib"; tst_init(); /* tst_ignore("// ** loaded"); */
  option(noloadLib);   // show loading of libraries;
                     // standard.lib is loaded
  listvar(package);
                     // the names of the procedures of inout.lib
  load("inout.lib"); // are now known to Singular
  listvar(package);
tst_status(1);$
