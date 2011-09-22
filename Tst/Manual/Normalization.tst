LIB "tst.lib"; tst_init();
  LIB "normal.lib";
  // ----- first example: rational quadruple point -----
  ring R=32003,(x,y,z),wp(3,5,15);
  ideal I=z*(y3-x5)+x10;
  list pr=normal(I);
  def S=pr[1][1];
  setring S;
  norid;
  // ----- second example: union of straight lines -----
  ring R1=0,(x,y,z),dp;
  ideal I=(x-y)*(x-z)*(y-z);
  list qr=normal(I);
  def S1=qr[1][1]; def S2=qr[1][2];
  setring S1; norid;
  setring S2; norid;
tst_status(1);$
