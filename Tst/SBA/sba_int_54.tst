LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 52y2z+32z3+4xy,xz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
