LIB "tst.lib"; tst_init();
LIB "cisimplicial.lib";
intmat A[2][5] = 60,0,140,150,21,0,60,140,150,21;
print(A);
printlevel = 0;
isCI(A);
printlevel = 1;
isCI(A);
intmat B[3][5] = 12,0,0,1,2,0,10,0,3,2,0,0,8,3,3;
print(B);
isCI(B);
printlevel=0;
tst_status(1);$
