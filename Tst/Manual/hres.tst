LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal I=xz,yz,x3-y3;
  def L=hres(I,0);
  print(betti(L),"betti");
  L[2];     // the first syzygy module of r/I
tst_status(1);$
