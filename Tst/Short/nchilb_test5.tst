LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";
  ring r=0,(x,y,z),dp;
  module p1=[1,x,z,y,z,x,z];
  module p2=[1,x,z,x];
  module p3=[1,x,z,y,z,z,x,z];
  module p4=[1,y,z];
  module p5=[1,x,z,z,x,z];
  list l1=list(p1,p2,p3,p4,p5);
  nchilb(l1,7);
tst_status(1);$
