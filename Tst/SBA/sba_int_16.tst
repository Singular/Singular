LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 53x2+62xy+17xz,3x2+52z2,72z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
