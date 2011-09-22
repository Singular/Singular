LIB "tst.lib"; tst_init();
  LIB "sample.lib";        // load the library sample.lib
  example tab;             // show an example
  "*"+tab(3)+"*";          // use the procedure tab
  // the static procedure internal_tab is not accessible
  "*"+internal_tab(3)+"*";
  // show the help section for tab
  help tab;
tst_status(1);$
