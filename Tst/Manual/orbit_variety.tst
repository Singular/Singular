LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix F[1][7]=x2+y2,z2,x4+y4,1,x2z-1y2z,xyz,x3y-1xy3;
string newring="E";
orbit_variety(F,newring);
print(G);
basering;
tst_status(1);$
