LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1),y(2),x(3),y(4),x(5..7),y(8)),dp;
list flag=identifyvar();
ideal J=x(1)^3*x(3)-y(2)*y(4)^2,x(5)*y(2)-x(7)*y(4)^2,x(6)^2*(1-y(4)*y(8)^5),x(7)^4*y(8)^2;
list L=data(J,4,8);
list Eo=EOrdlist(L[1],L[2],4,8,flag);
Eo[1]; // E-order
Eo[2]; // generator giving the E-order
Eo[3]; // term giving the E-order
tst_status(1);$
