LIB "tst.lib"; tst_init();
  int i=3;
  i=i+1;        // overriding
  i;
  int i=i+1;    // redefinition
  i;
tst_status(1);$
