LIB "tst.lib"; tst_init();
LIB "grobcov.lib";
ring R=(0,a1,a2,a3,a4),(x1,x2,x3,x4),dp;
ideal F=x4-a4+a2,
x1+x2+x3+x4-a1-a3-a4,
x1*x3*x4-a1*a3*a4,
x1*x3+x1*x4+x2*x3+x3*x4-a1*a4-a1*a3-a3*a4;
def T=cgsdrold(F,"old",0); "T="; T;
buildtreetoMaple(T,"Tb","Tb.txt");
tst_status(1);$
