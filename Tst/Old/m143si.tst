  ring r= 32003,(x,y),lp;
  ideal i=maxideal(1);
  attrib(i,"isSB",1);
  attrib(i);
  killattrib(i,"isSB");
  attrib(i);
LIB "tst.lib";tst_status(1);$
