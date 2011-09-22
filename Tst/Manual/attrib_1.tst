LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal I=std(maxideal(2));
  attrib(I,"isSB");
  // although maxideal(2) is a standard basis,
  // SINGULAR does not know it:
  attrib(maxideal(2), "isSB");
tst_status(1);$
