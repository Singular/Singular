LIB "tst.lib";
tst_init();

// ============================= example 1.9.25  ===============================

LIB "ncalg.lib";
LIB "central.lib";
def A0 = makeUsl2(); // U(sl_2) over the rationals
setring A0;
// compute the centralizer of f^2 up to degree 6
ideal C = centralizer(f^2,6); C;
ideal Z = center(5); Z;
def A5 = makeUsl2(5); // U(sl_2) over Z/5Z
setring A5;
ideal Z = center(5); 
Z;

// we are in the algebra A5
ideal T = twostd(4ef+h2-2h);  
T;
qring Q = T;
// compute the centralizer of f^2 up to degree 6
ideal C = centralizer(f^2,6); 
C;

tst_status(1);$

