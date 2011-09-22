LIB "tst.lib"; tst_init();
ring rs1=0,(x,y),lp;
poly f=15x5+x3+x2-10;
laguerre(f,10,2);
tst_status(1);$
