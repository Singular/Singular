LIB "tst.lib"; tst_init(); option(prot);

ring h2=32003,(t,x,y,z,w),dp;
ideal j=
2t3x5y2z+x2t5y+2x2y,
2y3z2x+3z2t6x2y+9y2t3z,
2t5z+y3x2t+z2t3y2x5;
ideal i=homog(j,w);
eliminate(i,yz);


tst_status(1);$
