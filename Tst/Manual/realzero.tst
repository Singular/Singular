LIB "tst.lib"; tst_init();
LIB "realrad.lib";
//in non parametric fields
ring r=0,(x,y),dp;
ideal i=(y3+3y2+y+1)*(y2+4y+4)*(x2+1),(x2+y)*(x2-y2)*(x2+2xy+y2)*(y2+y+1);
realzero(i);
ideal j=(y3+3y2+y+1)*(y2-2y+1),(x2+y)*(x2-y2);
realzero(j);
//to get every path
ring r1=(0,t),(x,y),lp;
ideal m1=x2+1-t,y3+t2;
ideal m2=x2+t2+1,y2+t;
ideal m3=x2+1-t,y2-t;
ideal m4=x^2+1+t,y2-t;
ideal i=intersect(m1,m2,m3,m4);
realzero(i);
tst_status(1);$
