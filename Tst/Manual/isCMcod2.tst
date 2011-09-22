LIB "tst.lib"; tst_init();
LIB "spcurve.lib";
ring r=32003,(x,y,z),ds;
ideal i=xz,yz,x^3-y^4;
print(isCMcod2(i));
tst_status(1);$
