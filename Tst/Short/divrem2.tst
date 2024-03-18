LIB "tst.lib";
tst_init();

ring r=QQ,(x,y),dp;
ideal a=x^3+y^3+x*y;
ideal b=x;
list L=system("DivRemIdU",a,b);
// a *u == b*q+r
matrix(a)*matrix(L[3])==matrix(b)*matrix(L[2])+matrix(L[1]);
ideal rest=NF(a,std(b));
rest;
L[1];
matrix(rest)==matrix(L[1]);

tst_status(1);$
