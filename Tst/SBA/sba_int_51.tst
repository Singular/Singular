LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 70x2z+3yz,60x3+77y2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
