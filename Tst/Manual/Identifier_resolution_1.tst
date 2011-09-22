LIB "tst.lib"; tst_init();
ring r=0,(x, x1),dp; // name x is substring of name x1 !!!!!!!!!
x;x1;   // resolved polynomial x
short=0; 2x1; // resolved to monomial 2*x^1 !!!!!!
2*x1; // resolved to product 2 times x1
tst_status(1);$
