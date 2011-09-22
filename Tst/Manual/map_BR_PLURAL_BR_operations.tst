LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def Usl2 = makeUsl2(); // this algebra is U(sl_2)
setring Usl2;
map F = Usl2, f, e, -h; // involutive endomorphism of U(sl_2)
F;
map G = F(F);
G;
poly p = (f+e*h)^2 + 3*h-e;
p;
F(p);
G(p);
(G(p) == p); // G is the identity
tst_status(1);$
