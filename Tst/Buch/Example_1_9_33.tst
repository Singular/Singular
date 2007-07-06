LIB "tst.lib";
tst_init();

============================== example 1.9.33 =============================

ring r = 0,(x,d,q),dp;
matrix D[3][3];
D[1,2]=q^2;
ncalgebra(1,D);
ideal I = x,d,q;
option(redSB); option(redTail);
resolution R = mres(I,0);
R;   
print(betti(R),"betti");

tst_status(1);$

