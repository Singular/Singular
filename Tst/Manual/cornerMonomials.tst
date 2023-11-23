LIB "tst.lib"; tst_init();
LIB "pointid.lib";
ring R = 0,x(1..3),ip;
poly n1 = 1;
poly n2 = x(1);
poly n3 = x(2);
poly n4 = x(1)^2;
poly n5 = x(3);
poly n6 = x(1)^3;
poly n7 = x(2)*x(3);
poly n8 = x(3)^2;
poly n9 = x(1)*x(2);
ideal N = n1,n2,n3,n4,n5,n6,n7,n8,n9;
cornerMonomials(N);
tst_status(1);$
