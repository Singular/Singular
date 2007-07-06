LIB "tst.lib";
tst_init();

================================== example 1.9.28 ====================

LIB "ncalg.lib";
def  U = makeUsl2(); // U(sl_2) over the rationals
setring U;
ring A = 0,(a),dp;
def  UA = U + A; 
setring UA;

poly  p = 4*e*f+h^2-2*h - a;
// p is a central element of UA
ideal I = e^3, f^3, h^3-4*h, p; 
// intersect I with the ring K[a]
ideal J = eliminate(I,e*f*h);
J;

eliminate(I,h);

eliminate(I,e*f*a);

tst_status(1);$

