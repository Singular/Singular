LIB "tst.lib"; tst_init();
LIB "latex.lib";
ring r0 = 0,(x,y),dp;                // char = 0, polynomial ordering
texring("",r0);
//
ring r7 =7,(x(0..2)),ds;             // char = 7, local ordering
texring("",r7);
//
ring r1 = 0,(x1,x2,y1,y2),wp(1,2,3,4);
texring("",r1);
//
ring rr = real,(x),dp;               // real numbers
texring("",rr);
//
ring rC = complex,x,dp;              // complex coefficients
texring("",rC);
//
ring rabc =(0,t1,t2,t3),(x,y),dp;    // ring with parameters
texring("",rabc);
//
ring ralg = (7,a),(x1,x2),ds;        // algebraic extension
minpoly = a2-a+3;
texring("",ralg);
texring("",ralg,"mipo");
//
ring r49=(49,a),x,dp;                // Galois field
texring("",r49);
//
setring r0;                          // quotient ring
ideal i = x2-y3;
qring q = std(i);
texring("",q);
//
// ------------------ additional features -------------------
ring r9 =0,(x(0..9)),ds;
texring("",r9,1);
texring("",r9,"C","{","^G");
//
ring rxy = 0,(x(1..5),y(1..6)),ds;
intvec v = 5,6;
texring("",rxy,v);
tst_status(1);$
