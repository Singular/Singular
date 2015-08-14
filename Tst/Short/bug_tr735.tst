LIB "tst.lib";
tst_init();

proc p2(alias def i) {listvar();i;}
int n;
p2(n);
ring r;
poly p=x;
p2(p);

proc p1(alias list j)
{
    def br = basering;
    listvar();
    ring s = 0, x, ds;
    list k = fetch(br, j);
    setring(br);
}
ring rr = 0, x, dp;
list l = list(ideal(x));
p1(l);


tst_status(1);$
