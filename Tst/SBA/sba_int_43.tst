LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 13x2y+48x2z+3xz2, 23xz2+8y3, 11yz2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
