LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 80x3,98x2y+61xy+75z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
