LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 33xy+46yz,40x2+18z2,98xy+13z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
