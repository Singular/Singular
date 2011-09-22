LIB "tst.lib"; tst_init();
  pyobject empty;
  empty;

  pyobject pystr = "Hello World!";
  pyobject pyone = 17;
  pyobject pylst = list(pystr, pyone);
  pylst;
tst_status(1);$
