LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring R = 0,(x,y,z),dp;
ideal I = x*(y-1),y,z*(y-1);
isReg(I,0);             // given list of generators is Q[x,y,z]-sequence
I = x*(y-1),z*(y-1),y;  // change sorting of generators
isReg(I,0);
ring r = 0,(x,y,z),ds;  // local ring
ideal I=fetch(R,I);
isReg(I,0);             // result independent of sorting of generators
tst_status(1);$
