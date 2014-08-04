LIB "tst.lib";
tst_init();

LIB("primdec.lib");
 
ring rng = 0,(x,y),dp;

ideal I = 1;
equidimMaxEHV(I);
ASSUME(0, idealsEqual( equidimMaxEHV(I), ideal(1) ));

I = 0;
equidimMaxEHV(I);
ASSUME(0, idealsEqual( equidimMaxEHV(I), ideal(0) ));


tst_status(1); $
