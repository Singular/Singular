LIB "tst.lib";
tst_init();

// missing operations with reals/bigints:

ring r10 = (real,10,20),x,dp;
 1/0;
0/0;
impart(0);
repart(0);

ring r = real,x,dp;
 1/0;
0/0;
impart(0);
repart(0);
ring r11 = (complex,i),x,dp;
 1/0;
0/0;
impart(0);
repart(0);

bigint M = 6;
 -M;
0-M;      // Nur als Subtraktion
-1*M;     // oder als Multilpikation moeglich
int m = 6;  // fuer int OK
-m;

// ----------------------------------------

kill M;
ring rr=0,(x,y,z),dp;
matrix M[1][3]=x,y,z;
ideal I=x2+1,y2+3,z2+5;
bigint p=3;

M+3*matrix(I);
M+p*matrix(I);

tst_status(1);$
