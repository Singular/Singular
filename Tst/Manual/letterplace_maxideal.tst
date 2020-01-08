LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = 0,(x,y,z),dp;
ring R = freeAlgebra(r, 10);
maxideal(0);
maxideal(1);
maxideal(4);

kill r;
kill R;

ring r = 0,(x,y,z),dp;
ring R = freeAlgebra(r, 10, 2);
maxideal(0);
maxideal(1);
maxideal(4);

tst_status(1);$
