LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1),y(2),x(3),y(4),x(5..7)),dp;
list flag=identifyvar();  // List giving flag=1 to invertible variables: y(2),y(4)
ideal J=x(1)^3-x(3)^2*y(4)^2,x(1)*x(7)*y(2)-x(6)^3*x(5)*y(4)^3,1-x(5)^2*y(2)^2;
list L=data(J,3,7);
list L2=maxEord(L[1],L[2],3,7,flag);
L2[1];     // Maximum E-order
list New=Nonhyp(L[1],L[2],3,7,flag,L2[2]);
New[1];    // Coefficients of the non hyperbolic part
New[2];    // Exponents of the non hyperbolic part
New[3];    // Coefficients of the hyperbolic part
New[4];    // New hyperbolic equations
New[5];    // New list giving flag=1 to invertible variables: y(2),y(4),y(5)
ring r = 0,(x(1..4)),dp;
list flag=identifyvar();
ideal J=1-x(1)^5*x(2)^2*x(3)^5, x(1)^2*x(3)^3+x(1)^4*x(4)^6;
list L=data(J,2,4);
list L2=maxEord(L[1],L[2],2,4,flag);
L2[1];     // Maximum E-order
list New=Nonhyp(L[1],L[2],2,4,flag,L2[2]);
New;
tst_status(1);$
