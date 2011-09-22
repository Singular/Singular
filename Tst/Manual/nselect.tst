LIB "tst.lib"; tst_init();
LIB "elim.lib";
ring r=0,(x,y,t,s,z),(c,dp);
ideal i=x-y,y-z2,z-t3,s-x+y3;
nselect(i,3);
module m=i*(gen(1)+gen(2));
m;
nselect(m,3..4);
nselect(matrix(m),3..4);
tst_status(1);$
