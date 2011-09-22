LIB "tst.lib"; tst_init();
  pyobject pystr = "python string ";
  pystr;
  pyobject pyint = 2;
  pyint;
  pyobject pylst = list(pystr, pyint);
  pylst;
  pyint + pyint;
  pyint * pyint;
  pystr + pystr;
  pystr * pyint;
  python_eval("17 + 4");
  typeof(_);
tst_status(1);$
