LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),ds;
monomialLcm(xy2,yz3);
monomialLcm([xy2,xz],[yz3]);
monomialLcm([xy2,xz3],[yz3]);
tst_status(1);$
