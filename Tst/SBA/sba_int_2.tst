LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 36y3+91z3+62y2+z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
