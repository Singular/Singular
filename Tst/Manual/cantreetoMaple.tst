LIB "tst.lib"; tst_init();
LIB "grobcov.lib";
ring R=(0,b,c,d,e,f),(x,y),dp;
ideal F=x^2+b*y^2+2*c*x*y+2*d*x+2*e*y+f, 2*x+2*c*y+2*d, 2*b*y+2*c*x+2*e;
def T=grobcovold(F,"out",1);
T;
cantreetoMaple(T,"Tm","Tm.txt");
tst_status(1);$
