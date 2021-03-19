LIB "tst.lib";
tst_init();

// fetch/imap alg.ext->algext misses minpoly:

LIB "normal.lib";
ring r = (0,a), (x,y,z), lp;
minpoly = a^2 + 1;
ideal I = a*z-x^4, z-y^6;
list nor = normal(I);
def s=nor[1][1];
setring s;
s;

list l=ringlist(s);
l[1][4];
setring r;
list l1=imap(s,l);
list l2=fetch(s,l);
l1[1][4];
l2[1][4];



tst_status(1);$
