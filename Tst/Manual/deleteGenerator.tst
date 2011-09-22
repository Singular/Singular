LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,z),dp;
ideal I = x^2,y^3,z^4;
deleteGenerator(I,2);
module M = [x,y,z],[x2,y2,z2],[x3,y3,z3];
print(deleteGenerator(M,2));
M = M[1];
deleteGenerator(M,1);
tst_status(1);$
