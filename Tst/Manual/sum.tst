LIB "tst.lib"; tst_init();
LIB "general.lib";
ring r1 = 0,(x,y,z),dp;
vector pv = [xy,xz,yz,x2,y2,z2];
sum(pv);
sum(pv,2..5);
matrix M[2][3] = 1,x,2,y,3,z;
intvec w=2,4,6;
sum(M,w);
intvec iv = 1,2,3,4,5,6,7,8,9;
sum(iv,2..4);
iv = intvec(1..100);
sum(iv);
ring r2 = 0,(x(1..10)),dp;
sum(x(3..7),intvec(1,3,5));
tst_status(1);$
