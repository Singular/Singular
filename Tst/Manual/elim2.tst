LIB "tst.lib"; tst_init();
LIB "elim.lib";
ring r=0,(x,y,u,v,w),dp;
ideal i=x-u,y-u2,w-u3,v-x+y3;
elim2(i,3..4);
module m=i*gen(1)+i*gen(2);
m=elim2(m,3..4);show(m);
tst_status(1);$
