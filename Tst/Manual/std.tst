LIB "tst.lib"; tst_init();
  // local computation
  ring r=32003,(x,y,z),ds;
  poly s1=1x2y+151xyz10+169y21;
  poly s2=1xz14+6x2y4+3z24;
  poly s3=5y10z10x+2y20z10+y10z20+11x3;
  ideal i=s1,s2,s3;
  ideal j=std(i);
  degree(j);
  // Hilbert driven elimination (standard)
  ring rhom=32003,(x,y,z,h),dp;
  ideal i=homog(imap(r,i),h);
  ideal j=std(i);
  bigintvec iv=hilb(j,1);
  ring rlex=32003,(x,y,z,h),lp;
  ideal i=fetch(rhom,i);
  ideal j=std(i,iv);
  j=subst(j,h,1);
  j[1];
  // Hilbert driven elimination (ideal is quasihomogeneous)
  intvec w=10,1,1;
  ring whom=32003,(x,y,z),wp(w);
  ideal i=fetch(r,i);
  ideal j=std(i);
  bigintvec iw=hilb(j,1,w);
  ring wlex=32003,(x,y,z),lp;
  ideal i=fetch(whom,i);
  ideal j=std(i,iw,w);
  j[1];
tst_status(1);$
