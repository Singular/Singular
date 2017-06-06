LIB "tst.lib";
tst_init();

// acccess to non-existent attributes of polys in ideals
ring R = 0,(x,y),dp;
ideal I = x+y;
link l = "ssi:w test.ssi";
write(l,I[1]);
close(l);

tst_status(1);$
