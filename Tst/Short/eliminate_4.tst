LIB "tst.lib"; tst_init(); option(prot);

ring h5=32003,(t,x,y,z,w),dp;
ideal j=
4t2z+6z3t+3z2,
5t2z7y3x+5x2z4t3y+3t,
6zt2y+2x+6z2y2t+2y;
ideal i=homog(j,w);

eliminate(i,t);

tst_status(1);$
