LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring r=32003,(b,s,t,u,v,w,x,y,z),dp;
ideal i=
bv+su,
bw+tu,
sw+tv,
by+sx,
bz+tx,
sz+ty,
uy+vx,
uz+wx,
vz+wy,
bvz;
idealSplit(i);
tst_status(1);$
