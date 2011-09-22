LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z),dp;
intmat M[3][2]=3,1,
3,2,
1,3;
normaliz(M,1);
intmat Hyperplanes[2][3] = 2,-1,0, // 2x-y >= 0
1, 1,0; //  x+y >= 0
intmat Equation[1][3] = 0,1,-1;    // y = z
intmat Congruence[1][4] = 1,0,0,3;  // x = 0 (3)
normaliz(Hyperplanes,4,Equation,5,Congruence,6);
tst_status(1);$
