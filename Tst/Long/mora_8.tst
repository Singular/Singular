LIB "tst.lib";
tst_init();
option(prot);

ring r=32003,(x,y,z,t,u,v),ds;
ideal i=
(y+u+v-zt)*(z+t+2u-3xt),
z2+t2+2u2-3u2v,
yx+t2+2yv-xyz,
xv-yu-zt-t2-u2-v2,
-1569yz3+31250x2tu,
-587yt+15625zv;
ideal j=std( i);
tst_status(1);$


