LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 199,(x,y,z,u,v),(c,ws(4,3,2,3,4));
ideal i    = xz-y2,yz2-xu,xv-yzu,yu-z3,z2u-yv,zv-u2;
//a cyclic quotient singularity
list L     = T_12(i,1);
print(L[5]);             //matrix of infin. deformations
printlevel = p;
tst_status(1);$
