LIB "tst.lib"; tst_init();

ring r=integer,x,dp;
ideal I=36;
mstd(I);

ring r=integer,(x,y),dp;

ideal J=36,2*x^3, 3*y^2;
// sb has 4 elements, second list entry should equal input
mstd(J);

ideal I=36,16x3;
// sb also has 2 elements, so it should appear as second list entry
mstd(I);

ideal K=3x3,2x5;
// sb also has 2 elements, so it should appear as second list entry
mstd(K);

ideal K=3x5,2x5;
// sb has one elements, so it should appear as second list entry
mstd(K);


ideal L=xy+y2,x3+x11;
// sb has 3 elements, second list entry should equal input
mstd(L);

ideal L=2xy+y2,3x3+x11;
// sb has 13 elements, second list entry should equal input
mstd(L);

ideal L=2xy+y2,3x3+x11,7x11,5y5;
mstd(L);

ideal L=2xy+y2,3x3+x11,7x11,5y5,3y4;;
mstd(L);

tst_status(1);$


