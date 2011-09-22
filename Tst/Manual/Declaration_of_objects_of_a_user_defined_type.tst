LIB "tst.lib"; tst_init();
  newstruct("nt","int a,poly b,string c");
  nt A;
  // as long as there is no value assigned to A.b, no ring is needed
  nt B=A;
tst_status(1);$
