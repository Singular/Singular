LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),dp;
matrix D[3][3];
D[1,2]=-z;
D[1,3]=y;
D[2,3]=x;
def S=nc_algebra(1,D); setring S;
ideal I = 0,x,0,1;
I;
I + 0;    // simplification
ideal J = I,0,x,x-z;
J;
I * J;   //  multiplication with simplification
I*x;
vector V = [x,y,z];
print(V*I);
ideal m = maxideal(1);
m^2;
ideal II = I[2..4];
II;
tst_status(1);$
