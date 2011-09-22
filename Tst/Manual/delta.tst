LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r = 32003,(x,y),ds;
poly f = x25+x24-4x23-1x22y+4x22+8x21y-2x21-12x20y-4x19y2+4x20+10x19y
+12x18y2-24x18y-20x17y2-4x16y3+x18+60x16y2+20x15y3-9x16y
-80x14y3-10x13y4+36x14y2+60x12y4+2x11y5-84x12y3-24x10y5
+126x10y4+4x8y6-126x8y5+84x6y6-36x4y7+9x2y8-1y9;
delta(f);
tst_status(1);$
