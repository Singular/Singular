LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
int plevel=printlevel;
printlevel=-1;
ring s=2,(x,y),lp;
list HC=Adj_div(x3+y2+y);
HC=NSplaces(1..2,HC);
HC=extcurve(2,HC);
def ER=HC[1][4];
setring ER;
intvec G=5;      // the rational divisor G = 5*HC[3][1]
intvec D=2..9;   // D = sum of the rational places no. 2..9 over F_4
// let us construct the corresponding evaluation AG code :
matrix C=AGcode_L(G,D,HC);
// here is a linear code of type [8,5,>=3] over F_4
print(C);
printlevel=plevel;
tst_status(1);$
