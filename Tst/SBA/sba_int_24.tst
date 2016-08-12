LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 82xy+y2,13y2,42x2+4xy+14y2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
