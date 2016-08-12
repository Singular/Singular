LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 100x2-2y2,3xyz-12z3,5yz2-5xy,25z2+100xy7;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
