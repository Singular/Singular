LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 35xz2+59z3+33xz+46z2,14x3+88xy2+74xy+59y2,71xyz+35xz2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
