  // ordering gives priority to components:
  ring rr=0,(x,y,z),(c,dp);
  vector v=[x2+y3,2,0,x*y]+gen(6)*x6;
  v;
  vector w=[z3-x,3y];
  v-w;
  v*(z+x);
LIB "tst.lib";tst_status(1);$
