LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
setNmzFilename("VeryInteresting");
intmat sgr[3][3]=1,2,3,4,5,6,7,8,10;
writeNmzData(sgr,1);
int dummy=system("sh","cat VeryInteresting.in");
intmat Hyperplanes[2][3] = 2,-1,0, // 2x-y >= 0
1, 1,0; //  x+y >= 0
intmat Equation[1][3] = 0,1,-1;    // y = z
intmat Congruence[1][4] = 1,0,0,3;  // x = 0 (3)
writeNmzData(Hyperplanes,4,Equation,5,Congruence,6);
dummy=system("sh","cat VeryInteresting.in");
tst_status(1);$
