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
// construct the corresp. residual AG code of type [8,3,>=5] over F_4:
matrix C=AGcode_Omega(G,D,HC);
// we can correct 1 error and the genus is 1, thus F must have degree 2
// and support disjoint from that of D;
intvec F=2;
list SV=prepSV(G,D,F,HC);
// now everything is prepared to decode with the basic algorithm;
// for example, here is a parity check matrix to compute the syndrome :
print(SV[1]);
// and here you have the correction capacity of the algorithm :
int epsilon=SV[size(D)+3][1];
epsilon;
printlevel=plevel;
tst_status(1);$
