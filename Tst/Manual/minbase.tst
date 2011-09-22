LIB "tst.lib"; tst_init();
  ring r=181,(x,y,z),(c,ls);
  ideal id2=x2+xyz,y2-z3y,z3+y5xz;
  ideal id4=maxideal(3)+id2;
  size(id4);
  minbase(id4);
tst_status(1);$
