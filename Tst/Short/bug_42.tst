LIB "tst.lib";
tst_init();
// error in radical with powers of polynomials in the ideal
LIB "primdec.lib";
ring r = (0, b, c, h), (a, d, e, f, g, i), (C, lp);
poly p1 =(b2)*i2+(-2bch)*i+(c2h2);
poly p2 =(c)*g2+gi2;
poly p3 =(h)*f+(c)*g+i2;
poly p4 =(h)*e+(b)*g+(h)*i;
poly p5 =d;
poly p6 =(c)*a+(b)*f+(c)*i;
ideal i = p1, p2, p3, p4, p5, p6;
zero_decomp(std(i), ideal(1), 0);
zero_decomp(std(i), ideal(1), 1);

tst_status(1);$
