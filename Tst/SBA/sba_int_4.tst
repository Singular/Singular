LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 76x2z,78xyz+10x2+38yz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
