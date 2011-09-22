LIB "tst.lib"; tst_init();
  python_run("def newfunc(*args): return list(args)");
  newfunc(1, 2, 3);          // newfunc also known to SINGULAR now

  python_run("import os");
  os;
  attrib(os, "name");
tst_status(1);$
