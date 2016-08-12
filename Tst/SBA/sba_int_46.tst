LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 29x2y+88y3+56xy,13xy2+24x2z+16yz,40x3+64x2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
