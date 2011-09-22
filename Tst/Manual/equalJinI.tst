LIB "tst.lib"; tst_init();
LIB "arcpoint.lib";
ring r=0,(x,y,z),dp;
ideal I = x,y+z2;
ideal J1= x;
ideal J2= x,y+z2;
equalJinI(I,J1);
equalJinI(I,J2);
tst_status(1);$
