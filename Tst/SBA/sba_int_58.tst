LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 62x2y+27xz2+18xy,18yz2,60x3+50xy+32y2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
