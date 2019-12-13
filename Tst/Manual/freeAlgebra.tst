LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = 0,(x,y,z),Dp;
ring R = freeAlgebra(r, 10);
isFreeAlgebra(R);
lpDegBound(R);
lpVarBlockSize(R);

kill r; kill R;

ring r = 0,(x,y,z),(c,dp);
ring R = freeAlgebra(r, 10, 5);
isFreeAlgebra(R);
lpDegBound(R);
lpVarBlockSize(R);

tst_status(1);$
