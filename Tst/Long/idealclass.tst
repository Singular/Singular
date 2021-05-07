LIB "tst.lib";
tst_init();

LIB"idealclass.lib";
ring R=0,(x,y),dp;
Ideal I=ideal(2x3+x2y-1,y2+3);
Ideal J=ideal(x+y,y2+1);
I+J;
I*J;
J^2;
string(I);


tst_status(1);$
