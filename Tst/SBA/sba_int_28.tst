LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 18xz,60yz+23z2,49xy+87y2+25z2;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
