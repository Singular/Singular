LIB "tst.lib"; tst_init();
LIB "elim.lib";
ring r=0,(x,y,t,s,z),(c,dp);
ideal i=x-y,y-z2,z-t3,s-x+y3;
ideal j=select(i,1);
j;
module m=i*(gen(1)+gen(2));
m;
select(m,1..2);
select(matrix(m),1..2);
tst_status(1);$
