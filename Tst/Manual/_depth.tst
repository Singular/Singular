LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring R=0,(x,y,z),dp;
ideal I=x2,xy,yz;
module M=0;
depth(M,I);   // depth(<x2,xy,yz>,Q[x,y,z])
ring r=0,(x,y,z),ds;  // local ring
matrix M[2][2]=x,xy,1+yz,0;
print(M);
depth(M);     // depth(maxideal,coker(M))
ideal I=x;
depth(M,I);   // depth(<x>,coker(M))
I=x+z;
depth(M,I);   // depth(<x+z>,coker(M))
tst_status(1);$
