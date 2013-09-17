LIB "tst.lib";
tst_init();

//  pNorm in Q(alpha)
ring r3mds =(0,a),(x),ds;
minpoly =  a^20-a^19-8*a^18-15*a^17+42*a^16+132*a^15-222*a^14-270*a^13+1047*a^12   +956*a^11-2420*a^10-2656*a^9+2982*a^8+2049*a^7+3012*a^6+2721*a^5+747*a^4-294*a^3
+496*a^2-88*a+16;
factorize(x2+3);


tst_status(1);$
