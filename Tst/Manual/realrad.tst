LIB "tst.lib"; tst_init();
LIB "realrad.lib";
ring r1=0,(x,y,z),lp;
//dimension 0
ideal i0=(x2+1)*(x3-2),(y3-2)*(y2+y+1),z3+2;
//dimension 1
ideal i1=(y3+3y2+y+1)*(y2+4y+4)*(x2+1),(x2+y)*(x2-y2)*(x2+2xy+y2)*(y2+y+1);
ideal i=intersect(i0,i1);
realrad(i);
tst_status(1);$
