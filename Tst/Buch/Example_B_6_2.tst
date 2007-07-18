LIB "tst.lib";
tst_init();

// ======================================= example B.6.2 ===================

ring R=0,(x,y,z),dp;
poly f=x4+(-z+3)*x3+(z3+(y-3)*z-y2-13)*x2
         +(-z4+(y2+3y+15)*z+6)*x
         +yz4+2z3+z*(-y3-15y)-2y2-30;
factorize(f);

tst_status(1);$

