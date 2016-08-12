LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 38xy+64z2,100y2,16xy+17xz+yz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
