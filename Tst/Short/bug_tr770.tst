LIB "tst.lib";
tst_init();

LIB"brnoeth.lib";

printlevel = -5;
ring rng = 2,(x,y,z),dp;
short = 0;
poly p = x^7 + y^7 + z^7;
def result = Adj_div(p);

tst_status(1); $;

