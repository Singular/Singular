LIB "tst.lib"; tst_init();
LIB "hdepth.lib";

ring R = 0,t,ds;
poly f = 2-3t-2t2+2t3+4t4;
hdepth_p(f,5,0);
hdepth_p(f,5,1);
f = 5t2-5t3+t5;
hdepth_p(f,3,0);
f = 1-3t+6*t^3;
hdepth_p(f,10,0);

ring R = 0,(x(1..10)),dp;
ideal i=maxideal(1);
module M=i;
hdepth(M);
hdepth(M,0);
hdepth(M,1);

ring R = 0,(x,y,z),dp;
ideal i = x2,xy,xz,y2,z2;
module m = i;
hdepth(m,0);

tst_status(1);$
