LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal I=maxideal(2); // the attribute "isSB" is set
  vdim(I); // no warn!
  ideal J=I[1..ncols(I)]; // the attribute "isSB" is not set
  vdim(J); // warn!
  attrib(J,"isSB",1);  // the standard basis attribute is set here
  vdim(J);
tst_status(1);$
