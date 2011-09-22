LIB "tst.lib"; tst_init();
LIB "homolog.lib";
int p      = printlevel;
ring  rr   = 32003,(x,y,z),(dp,C);
ideal  I   = x4+y3+z2;
qring  o   = std(I);
module M   = [x,y,0,z],[y2,-x3,z,0],[z,0,-y,-x3],[0,z,x,-y2];
print(cup(M));
print(cup(M,1));
// 2nd EXAMPLE  (shows what is going on)
printlevel = 3;
ring   r   = 0,(x,y),(dp,C);
ideal  i   = x2-y3;
qring  q   = std(i);
module M   = [-x,y],[-y2,x];
print(cup(M));
printlevel = p;
tst_status(1);$
