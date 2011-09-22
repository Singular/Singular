LIB "tst.lib"; tst_init();
  int foo(1)=42;
  string bar="foo";
  `bar+"(1)"`;
tst_status(1);$
