LIB "tst.lib"; tst_init();
LIB "deform.lib";
int p          = printlevel;
printlevel     = 0;
ring r1        = 0,(x,y,z,u,v),ds;
matrix m[2][4] = x,y,z,u,y,z,u,v;
ideal Fo       = minor(m,2);
// cone over rational normal curve of degree 4
list L=versal(Fo);
L;
def Px=L[1];
setring Px;
// ___ Equations of miniversal base space ___:
Js;"";
// ___ Equations of miniversal total space ___:
Fs;"";
tst_status(1);$
