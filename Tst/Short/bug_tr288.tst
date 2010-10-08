LIB "tst.lib";
tst_init();

// missed cancelunit for 1+y (ecart==0)

ring r = 0, (x, y), (a(1, -1), dp); x+y+1;
std(ideal(1+y));
std(ideal(1+y+x));
option(redSB);
std(ideal(1+y));
std(ideal(1+y+x));

tst_status(1);$
