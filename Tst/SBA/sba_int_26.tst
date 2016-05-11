LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 14x2,77x2+82xy,20xy+4y2+59yz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
