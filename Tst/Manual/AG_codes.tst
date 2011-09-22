LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
ring s=2,(x,y),lp;            // characteristic 2
poly f=x3y+y3+x;              // the Klein quartic
list KLEIN=Adj_div(f);        // compute the conductor
KLEIN=NSplaces(1..3,KLEIN);   // computes places up to degree 3
KLEIN=extcurve(3,KLEIN);      // construct Klein quartic over F_8
KLEIN[3];                     // display places (degree, number)
// We define a divisor G of degree 14=6*1+4*2:
intvec G=6,0,0,4,0,0,0,0,0,0,0;   // 6 * place #1 + 4 * place #4
// We compute an evaluation code which evaluates at all rational places
// outside the support of G (place #4 is not rational)
intvec D=2..24;
// in D, the number i refers to the i-th element of the list POINTS in
// the ring KLEIN[1][5].
def RR=KLEIN[1][5];
setring RR; POINTS[1];        // the place in the support of G (not in supp(D))
setring s;
def RR=KLEIN[1][4];
setring RR;
matrix C=AGcode_L(G,D,KLEIN); // generator matrix for the evaluation AG code
nrows(C);
ncols(C);
//
// We can also compute a generator matrix for the residual AG code
matrix CO=AGcode_Omega(G,D,KLEIN);
//
// Preparation for decoding:
// We need a divisor of degree at least 6 whose support is disjoint with the
// support of D:
intvec F=6;                   // F = 6*point #1
// in F, the i-th entry refers to the i-th element of the list POINTS in
// the ring KLEIN[1][5]
list K=prepSV(G,D,F,KLEIN);
K[size(K)][1];                // error-correcting capacity
//
//  Encoding and Decoding:
matrix word[1][11];           // a word of length 11 is encoded
word = 1,1,1,1,1,1,1,1,1,1,1;
def y=word*CO;                // the code word (length: 23)
matrix disturb[1][23];
disturb[1,1]=1;
disturb[1,10]=a;
disturb[1,12]=1+a;
y=y+disturb;                  // disturb the code word (3 errors)
def yy=decodeSV(y,K);         // error correction
yy-y;                         // display the error
tst_status(1);$
