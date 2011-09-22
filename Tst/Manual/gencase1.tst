LIB "tst.lib"; tst_init();
LIB "grobcov.lib";
"Generic segment for the extended Steiner-Lehmus theorem";
ring R=(0,x,y),(a,b,m,n,p,r),lp;
ideal S=p^2-(x^2+y^2),
-a*(y)+b*(x+p),
-a*y+b*(x-1)+y,
(r-1)^2-((x-1)^2+y^2),
-m*(y)+n*(x+r-2) +y,
-m*y+n*x,
(a^2+b^2)-((m-1)^2+n^2);
short=0;
gencase1(S);
tst_status(1);$
