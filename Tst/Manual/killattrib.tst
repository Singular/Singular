LIB "tst.lib"; tst_init();
  ring r=32003,(x,y),lp;
  ideal i=maxideal(1);
  attrib(i,"isSB",1);
  attrib(i);
  killattrib(i,"isSB");
  attrib(i);
tst_status(1);$
