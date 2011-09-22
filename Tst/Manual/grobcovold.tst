LIB "tst.lib"; tst_init();
LIB "grobcov.lib";
"Simple robot: A. Montes,";
"New algorithm for discussing Groebner bases with parameters,";
"JSC, 33: 183-208 (2002).";
ring R=(0,r,z,l),(s1,c1,s2,c2), dp;
ideal S10=c1^2+s1^2-1,
c2^2+s2^2-1,
r-c1-l*c1*c2+l*s1*s2,
z-s1-l*c1*s2-l*s1*c2;
grobcovold(S10,"comment",1);
grobcovold(S10,"can",2,"comment",1);
tst_status(1);$
