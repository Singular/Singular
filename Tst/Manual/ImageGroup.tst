LIB "tst.lib"; tst_init();
LIB "rinvar.lib";
ring B   = 0,(s(1..2), t(1..2)),dp;
ideal G = s(1)^3-1, s(2)^10-1;
ideal action = s(1)*s(2)^8*t(1), s(1)*s(2)^7*t(2);
def R = ImageGroup(G, action);
setring R;
groupid;
actionid;
tst_status(1);$
