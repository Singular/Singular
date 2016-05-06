LIB "tst.lib";
tst_init();

// conversion list -> resolution must include attribute isHomog
ring r = 0, x, dp;
module m = 0;
attrib(m, "isHomog", intvec(3));
print(betti(list(m), 0), "betti");
attrib(m,"isHomog");
resolution s = list(m);
attrib(s);
attrib(s,"isHomog");
print(betti(s, 0), "betti");

tst_status(1);$;


