LIB "tst.lib";
tst_init();

ring A = 0,(x(1..4),t,y(1..2)),lp;

ideal I   =x(1)^2-x(2)^3; 
poly f1,f2=x(3)^2-1,x(1)^2*x(2);
poly b    =x(3);
ideal M   =t-b,y(1)-f1,y(2)-f2,I;

groebner(M);

b =x(4);
M =t-b,y(1)-f1,y(2)-f2,I;

groebner(M);

tst_status(1);$
