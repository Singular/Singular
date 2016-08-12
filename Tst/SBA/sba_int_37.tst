LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 22x2+91xy+11xz,77xz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
