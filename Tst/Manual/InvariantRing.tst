LIB "tst.lib"; tst_init();
LIB "rinvar.lib";
ring B = 0, (s(1..2), t(1..2)), dp;
ideal G = -s(1)+s(2)^3, s(1)^4-1;
ideal action = s(1)*t(1), s(2)*t(2);
def R = InvariantRing(std(G), action);
setring R;
invars;
tst_status(1);$
