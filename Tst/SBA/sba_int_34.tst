LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 96x2,44y2+26xz+15z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
