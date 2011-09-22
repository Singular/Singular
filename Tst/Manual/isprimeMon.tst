LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z,t),lp;
ideal I = w,y,t;
isprimeMon (I);
ideal J = w,y,t,x*z;
isprimeMon (J);
tst_status(1);$
