LIB "tst.lib";
tst_init();

LIB "gaussman.lib";

ring R=0,(x,y),ds;
poly f=x5+x2y2+y5;

print(monomat(f));
print(monospec(f));
list l=vfilt(f);
l;
vjacob(l);
l=singspec(f);
l;
gamma(l);
gamma4(l);

tst_status(1);
$
