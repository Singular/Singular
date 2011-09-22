LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r  = 0,(x,y),dp;
ideal j = y6,x4;
ideal m = x,y;
attrib(m,"isSB",1);  //let Singular know that ideals are a standard basis
attrib(j,"isSB",1);
codim(m,j);          // should be 23 (Milnor number -1 of y7-x5)
tst_status(1);$
