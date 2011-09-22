LIB "tst.lib"; tst_init();
  // ordering gives priority to components:
  ring rr=0,(x,y,z),(c,dp);
  vector v=[x2+y3,2,0,x*y]+gen(6)*x6;
  v;
  vector w=[z3-x,3y];
  v-w;
  v*(z+x);
  // ordering gives priority to monomials:
  // this results in a different output
  ring r=0,(x,y,z),(dp,c);
  imap(rr,v);
tst_status(1);$
