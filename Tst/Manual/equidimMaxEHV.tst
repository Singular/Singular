LIB "tst.lib"; tst_init();
LIB "primdec.lib";
ring  r = 0,(x,y,z),dp;
ideal i=intersect(ideal(z),ideal(x,y),ideal(x2,z2),ideal(x5,y5,z5));
equidimMaxEHV(i);
tst_status(1);$
