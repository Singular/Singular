LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 69x2+4xy+35yz,66xy+72z2,54y2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
