LIB "tst.lib"; tst_init();

ring RR = 0, (x,y,z), dp;
list L = ringlist(RR);
L[1] = list(0, list("a"), list(list("lp", 1)), ideal(0));
def RTemp = ring(L);
setring RTemp;
list L = ringlist(RTemp);
L[1][4] = ideal(a^2 - 2);

def R = ring(L); // projection of qideal in rCompose - wrong!?

setring R; R;

tst_status(1);$