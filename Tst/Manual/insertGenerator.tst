LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,z),dp;
ideal I = x^2,z^4;
insertGenerator(I,y^3);
insertGenerator(I,y^3,2);
module M = I*gen(3);
insertGenerator(M,[x^3,y^2,z],2);
insertGenerator(M,x+y+z,4);
tst_status(1);$
