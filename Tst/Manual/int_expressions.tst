LIB "tst.lib"; tst_init();
12345678901; // too large
typeof(_);
ring r=0,x,dp;
12345678901;
typeof(_);
// Note: 11*13*17*100*200*2000*503*1111*222222
// returns a machine integer:
11*13*17*100*200*2000*503*1111*222222;
// using the type cast number for a greater allowed range
number(11)*13*17*100*200*2000*503*1111*222222;
ring rp=32003,x,dp;
12345678901;
typeof(_);
intmat m[2][2] = 1,2,3,4;
m;
m[2,2];
typeof(_);
det(m);
m[1,1] + m[2,1] == trace(m);
! 0;
1 and 2;
intvec v = 1,2,3;
def d =transpose(v)*v;    // scalarproduct gives an 1x1 intvec
typeof(d);
int i = d[1];             // access the first (the only) entry in the intvec
ring rr=31,(x,y,z),dp;
poly f = 1;
i = int(f);               // cast to int
// Integers may be converted to constant  polynomials by an assignment,
poly g=37;
// define the constant polynomial g equal to the image of
// the integer 37 in the actual coefficient field, here it equals 6
g;
tst_status(1);$
