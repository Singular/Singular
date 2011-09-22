LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,z,s),Dp;
ideal I = -1,-4/3,0,-5/3,-2;
intvec mI = 2,1,2,1,1;
list BS = I,mI;
poly p = fl2poly(BS,"s");
p;
factorize(p,2);
tst_status(1);$
