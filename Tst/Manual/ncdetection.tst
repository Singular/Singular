LIB "tst.lib"; tst_init();
LIB "involut.lib";
ring R = 0,(x,y,z,D(1..3)),dp;
matrix D[6][6];
D[1,4]=1; D[2,5]=1;  D[3,6]=1;
def r = nc_algebra(1,D); setring r;
ncdetection();
kill r, R;
//----------------------------------------
ring R=0,(x,S),dp;
def r = nc_algebra(1,-S); setring r;
ncdetection();
kill r, R;
//----------------------------------------
ring R=0,(x,D(1),S),dp;
matrix D[3][3];
D[1,2]=1;  D[1,3]=-S;
def r = nc_algebra(1,D); setring r;
ncdetection();
tst_status(1);$
