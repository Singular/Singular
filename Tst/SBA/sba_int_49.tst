LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 20yz2+23y2+13z2,25xy2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
