LIB "tst.lib"; tst_init();
LIB"freegb.lib";

ring r = 0,(x,y),Dp;
ring R = freeAlgebra(r, 5);
lpLmDivides(1, x) == 1;
lpLmDivides(x, 1) == 0;
lpLmDivides(x*y, x) == 0;
lpLmDivides(x*y, y) == 0;
lpLmDivides(x*y, x*x) == 0;
lpLmDivides(x*y, y*x) == 0;
lpLmDivides(x*y, x*y) == 1;
lpLmDivides(x*y, x*x*x*x) == 0;
lpLmDivides(x*y, y*x*x*x) == 0;
lpLmDivides(x*y, x*y*x*x) == 1;
lpLmDivides(x*y, x*x*y*x) == 1;
lpLmDivides(x*y, x*x*x*y) == 1;
lpLmDivides(x*y, x*x*x*x*x) == 0;
lpLmDivides(x*y, y*x*x*x*x) == 0;
lpLmDivides(x*y, x*y*x*x*x) == 1;
lpLmDivides(x*y, x*x*y*x*x) == 1;
lpLmDivides(x*y, x*x*x*y*x) == 1;
lpLmDivides(x*y, x*x*x*x*y) == 1;

tst_status(1);$
