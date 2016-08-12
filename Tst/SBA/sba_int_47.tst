LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 95x2y+82x2,3xyz,29x2z+62xy;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
