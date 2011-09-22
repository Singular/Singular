LIB "tst.lib"; tst_init();
LIB "elim.lib";
ring r=0,(x,y,t,s,z),dp;
ideal i=x-t,y-t2,z-t3,s-x+y3;
elim1(i,ts);
module m=i*gen(1)+i*gen(2);
m=elim1(m,3..4); show(m);
tst_status(1);$
