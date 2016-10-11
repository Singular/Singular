LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";

  ring r=0,(x,y,z),dp;
  module p1=[1,y,z],[-1,z,y];
  module p2=[1,x,z,x],[-1,z,x,z];
  list l1=list(p1,p2);
  nchilb(l1,6,1);
tst_status(1);$
