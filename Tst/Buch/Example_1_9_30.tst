LIB "tst.lib";
tst_init();

=============================== example 1.9.30 ==============================

LIB "ncalg.lib";
def A0 = makeUsl2();  setring A0;
option(redSB);  option(redTail);
ideal I = e2,f2,h2-1;
I = twostd(I);
print(matrix(I));     // ideal in a compact form

qring A  = I;        // we move to a GR--algebra A
ideal Ke = modulo(e,0);
Ke = std(Ke+std(0)); // normalize Ke w.r.t. the factor ideal
Ke;

ideal Kh = modulo(h-1,0);
Kh = std(Kh+std(0));
Kh;                  // the kernel of h-1

tst_status(1);$

