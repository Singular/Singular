LIB "tst.lib";
tst_init();

// off by one in regularity (anne)
ring R=0,(u,v,x,y,z),dp;
regularity(mres(ideal(ux,vx,uy,vy),0));

tst_status(1);$
