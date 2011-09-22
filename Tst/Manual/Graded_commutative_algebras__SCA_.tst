LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r = 0,(a, b, x,y,z, Q, W),(lp(2), dp(3), Dp(2));
// Let us make variables x = var(3), ..., z = var(5) to be anti-commutative
// and add additionally a quotient ideal:
def S = superCommutative(3, 5, ideal(a*W + b*Q*x + z) ); setring S; S;
ideal I = a*x*y + z*Q + b, y*Q + a; I;
std(I); // Groebner basis is used here since > is global
kill r;
// Let's do the same but this time with some local commutative variables:
ring r = 0,(a, b, x,y,z, Q, W),(dp(1), ds(1), lp(3), ds(2));
def S = superCommutative(3, 5, ideal(a*W + b*Q*x + z) ); setring S; S;
ideal I = a*x*y + z*Q + b, y*Q + a; I;
std(I);
tst_status(1);$
