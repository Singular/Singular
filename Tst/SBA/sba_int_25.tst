LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 74xz+64yz,12xy+56xz+64yz,92yz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
