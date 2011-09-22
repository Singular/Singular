LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix F[1][3]=x+y+z,xy+xz+yz,xyz;
ideal I=x2+y2+z2-1,x2y+y2z+z2x-2x-2y-2z,xy2+yz2+zx2-2x-2y-2z;
string newring="E";
relative_orbit_variety(I,F,newring);
print(G);
basering;
tst_status(1);$
