  ring r=0,(x,y,z),dp;
  ideal i=x2+z,z,2z;
  option(redSB);
  interred(i);
  ring R=0,(x,y,z),ds;
  ideal i=zx+y3,z+y3,z+xy;
  interred(i);
LIB "tst.lib";tst_status(1);$
