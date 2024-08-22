LIB "tst.lib"; tst_init();
// unit ideal for absPrimdecGTZ
ring R = 0, (x, y), dp;
ideal I = x + 1, y + 1, y;
LIB "primdec.lib";
def L0=absPrimdecGTZ(I);
typeof(L0);
def L1=absPrimdecGTZ(ideal(x));
typeof(L1);
def L2=absPrimdecGTZ(ideal(x*y));
typeof(L2);
tst_status(1);$

