LIB "tst.lib";
tst_init();

ring R=0,x,dp;
poly f=4x2;
factorize(f);f;
factorize(f,0);f;
factorize(f,1);f;
factorize(f,2);f;
factorize(f,3);f;
factorize(f,4);f;

tst_status(1);$
