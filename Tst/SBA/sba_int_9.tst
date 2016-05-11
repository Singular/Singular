LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 36yz2+13xz+99z2,43x3,68xy2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
