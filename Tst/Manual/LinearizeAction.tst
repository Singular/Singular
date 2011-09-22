LIB "tst.lib"; tst_init();
LIB "rinvar.lib";
ring B   = 0,(s(1..5), t(1..3)),dp;
ideal G =  s(3)-s(4), s(2)-s(5), s(4)*s(5), s(1)^2*s(4)+s(1)^2*s(5)-1, s(1)^2*s(5)^2-s(5), s(4)^4-s(5)^4+s(1)^2, s(1)^4+s(4)^3-s(5)^3, s(5)^5-s(1)^2*s(5);
ideal action = -s(4)*t(1)+s(5)*t(1), -s(4)^2*t(2)+2*s(4)^2*t(3)^2+s(5)^2*t(2), s(4)*t(3)+s(5)*t(3);
LinearActionQ(action, 5);
def R = LinearizeAction(G, action, 5);
setring R;
R;
actionid;
embedid;
groupid;
LinearActionQ(actionid, 5);
tst_status(1);$
