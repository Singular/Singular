LIB "tst.lib"; tst_init();
LIB "alexpoly.lib";
ring r=0,(x,y),ds;
list INV=invariants((x2-y3)*(x3-y2)*((x2-y3)^2-4x5y-x7));
intmat contact=INV[4][1];
list charexp=INV[1][1],INV[2][1],INV[3][1];
// The intersection matrix is INV[4][2].
print(INV[4][2]);
// And it is calulated as ...
print(charexp2inter(contact,charexp));
tst_status(1);$
