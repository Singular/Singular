LIB "tst.lib"; tst_init();
LIB "realrad.lib";
ring r1 = 0,x,dp;
poly f=x5+16x2+x+1;
realpoly(f);
realpoly(f*(x4+2));
ring r2=0,(x,y),dp;
poly f=x6-3x4y2 + y6 + x2y2 -6y+5;
realpoly(f);
ring r3=0,(x,y,z),dp;
poly f=x4y4-2x5y3z2+x6y2z4+2x2y3z-4x3y2z3+2x4yz5+z2y2-2z4yx+z6x2;
realpoly(f);
realpoly(f*(x2+y2+1));
tst_status(1);$
