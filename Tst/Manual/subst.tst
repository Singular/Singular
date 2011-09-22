LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  poly f=x2+y2+z2+x+y+z;
  subst(f,x,y,y,z);   // first substitute x by y, then y by z
  subst(f,y,z,x,y);   // first substitute y by z, then x by y
tst_status(1);$
