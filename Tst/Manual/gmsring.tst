LIB "tst.lib"; tst_init();
LIB "gmssing.lib";
ring @R=0,(x,y),ds;
poly t=x5+x2y2+y5;
def G=gmsring(t,"s");
setring(G);
gmspoly;
print(gmsjacob);
print(gmsstd);
print(gmsmatrix);
print(gmsbasis);
gmsmaxdeg;
tst_status(1);$
