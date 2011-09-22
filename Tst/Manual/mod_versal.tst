LIB "tst.lib"; tst_init();
LIB "deform.lib";
int p = printlevel;
printlevel = 1;
ring  Ro = 0,(x,y),wp(3,4);
ideal Io = x4+y3;
matrix Mo[2][2] = x2,y,-y2,x2;
list L = mod_versal(Mo,Io);
def Qx=L[2]; setring Qx;
print(Ms);
print(Ls);
print(Js);
printlevel = p;
if (defined(Px)) {kill Px,Qx,So;}
tst_status(1);$
