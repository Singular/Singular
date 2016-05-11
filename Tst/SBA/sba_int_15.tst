LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 70x2+26y2,62xy+31y2,58xy+41z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
