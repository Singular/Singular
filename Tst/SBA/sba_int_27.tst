LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 66yz2+69yz+50z2,27y3+93xz2,64x2y+47x2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
