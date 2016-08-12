LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 92y2z,64x2y+64x2+24yz+37z2,48x3+12xyz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
