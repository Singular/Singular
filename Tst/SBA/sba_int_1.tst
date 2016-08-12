LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 56xz,50xyz+35xz2+20z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
