LIB "tst.lib";
tst_init();

// off by one in regularity (anne)
ring R=0,(u,v,x,y,z),dp;
// should be 2 -1
regularity(mres(ideal(ux,vx,uy,vy),0));
// should be 5 (nvars) -1
regularity(mres(ideal(uvxyz),0));
ring r=0,(x1,x2,x3,x4,x5),dp;
// should be 3 -1
regularity(mres(ideal(x1*x3,x2*x4,x1*x2*x5,x2*x3*x5,x3*x4*x5,x1*x4*x5),0));
ring rr=0,(y1,y2,y3,x1,x2,x3,x4),dp;
// should be 2 -1
regularity(mres(ideal(y1*x1,y1*x2,y1*x3,y1*x4,
                      y2*x1,y2*x2,y2*x3,y2*x4,
		      y3*x1,y3*x2,y3*x3,y3*x4,
		      x3*x4),0));

tst_status(1);$
