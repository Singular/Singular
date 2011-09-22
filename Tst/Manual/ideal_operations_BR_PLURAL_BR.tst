LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),dp;
matrix D[3][3];
D[1,2]=-z;  D[1,3]=y;  D[2,3]=x;
def R=nc_algebra(1,D); // this algebra is U(so_3)
setring R;
ideal I = 0,x,0,1;
I;
I + 0;    // simplification
I*x;
ideal J = I,0,x,x-z;
I * J;   //  multiplication with simplification
vector V = [x,y,z];
print(I*V);
ideal m = maxideal(1);
m^2;
ideal II = I[2..4];
II;
tst_status(1);$
