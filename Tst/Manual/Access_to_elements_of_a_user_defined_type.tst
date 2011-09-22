LIB "tst.lib"; tst_init();
  newstruct("nt","int a,poly b,string c");
  nt A;
  3+A.a;
  A.c="example string";
  ring r;
  A.b=poly(1); // assignment: expression must be of the given type
  A;
tst_status(1);$
