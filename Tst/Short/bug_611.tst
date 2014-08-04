LIB "tst.lib";
tst_init();

LIB("primdec.lib");

ring rng = 0,(x,y),dp;
ideal J = 0;
list l = prepareAss(J); 
l;


tst_status(1); $
