LIB "tst.lib"; tst_init();
ring A = 0,(x), dp;

ideal I = x; I = std(I);

resolution M=nres(I,0);
list l = M;
l;
type(l[1]);
print(matrix(l[1]));
transpose(l[1]);


tst_status(1);$
