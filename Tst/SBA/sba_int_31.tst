LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 35xy,76y2+37yz,95z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
