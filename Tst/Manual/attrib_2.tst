LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal I=maxideal(2); // the attribute "isSB" is not set
  vdim(I);
  attrib(I,"isSB",1);  // the standard basis attribute is set here
  vdim(I);
tst_status(1);$
