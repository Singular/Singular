LIB "tst.lib";
tst_init();

ring A = 0,(x,y,z),lp;
poly f = y4z3+2x2y2z2+3x5+4z4+5y2;
f;

leadmonom(f);
leadexp(f); 
lead(f);
leadcoef(f); 
f - lead(f);

tst_status(1);$
