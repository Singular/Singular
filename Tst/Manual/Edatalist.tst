LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1),y(2),x(3),y(4),x(5..7),y(8)),dp;
list flag=identifyvar();
ideal J=x(1)^3*x(3)-y(2)*y(4)^2,x(5)*y(2)-x(7)*y(4)^2,x(6)^2*(1-y(4)*y(8)^5);
list L=data(J,3,8);
list EL=Edatalist(L[1],L[2],3,8,flag);
EL; // E-order of each term
ring r = 2,(x(1),y(2),x(3),y(4),x(5..7),y(8)),dp;
list flag=identifyvar();
ideal J=x(1)^3*x(3)-y(2)*y(4)^2,x(5)*y(2)-x(7)*y(4)^2,x(6)^2*(1-y(4)*y(8)^5);
list L=data(J,3,8);
list EL=Edatalist(L[1],L[2],3,8,flag);
EL; // E-order of each term IN CHAR 2, COMPUTATIONS NEED TO BE DONE IN CHAR 0
ring r = 0,(x(1..3)),dp;
list flag=identifyvar();
ideal J=x(1)^4*x(2)^2, x(1)^2-x(3)^3;
list L=data(J,2,3);
list EL=Edatalist(L[1],L[2],2,3,flag);
EL; // E-order of each term
tst_status(1);$
