LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 52xyz+7yz,27x2z+95yz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
