LIB "tst.lib"; tst_init();
LIB "homolog.lib";
int p      = printlevel;
ring  rr   = 32003,(x,y,z),(dp,C);
ideal  I   = x4+y3+z2;
qring  o   = std(I);
module M   = [x,y,0,z],[y2,-x3,z,0],[z,0,-y,-x3],[0,z,x,-y2];
print(cupproduct(M,M,M,1,3));
printlevel = 3;
list l     = (cupproduct(M,M,M,1,3,"any"));
show(l[1]);show(l[2]);
printlevel = p;
tst_status(1);$
