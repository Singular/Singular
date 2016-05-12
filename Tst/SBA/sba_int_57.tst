LIB "tst.lib";
tst_init();

ring r = (integer),(x,y,z),dp;
ideal i = 61z3,44y3+70y2z+73y2,19xy2,72x2+yz;
ideal stdi = std(i);
ideal sbai = sba(i,1,0);
std(reduce(stdi,sbai));
std(reduce(sbai,stdi));


tst_status(1);
$
