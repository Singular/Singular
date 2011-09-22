LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring R = 0,(x,y),dp;
ring S = 0,(u,v),ls;
ord_test(R);
ord_test(S);
ord_test(R+S);
tst_status(1);$
