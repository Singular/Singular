LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 68yz+76z2,28x2+28xy,68xy+7yz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
