LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r  = 0,(u,v,w,x,y,z),dp;
ideal i = wx,wy,wz,vx,vy,vz,ux,uy,uz,y3-x2;;
slocus(i);
tst_status(1);$
