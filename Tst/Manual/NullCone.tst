LIB "tst.lib"; tst_init();
LIB "rinvar.lib";
ring R = 0, (s(1..2), x, y), dp;
ideal G = -s(1)+s(2)^3, s(1)^4-1;
ideal action = s(1)*x, s(2)*y;
ideal inv = NullCone(G, action);
inv;
tst_status(1);$
