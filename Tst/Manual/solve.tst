LIB "tst.lib"; tst_init();
LIB "solve.lib";
// Find all roots of a multivariate ideal using triangular sets:
int d,t,s = 4,3,2 ;
int i;
ring A=0,x(1..d),dp;
poly p=-1;
for (i=d; i>0; i--) { p=p+x(i)^s; }
ideal I = x(d)^t-x(d)^s+p;
for (i=d-1; i>0; i--) { I=x(i)^t-x(i)^s+p,I; }
I;
// the multiplicity is
vdim(std(I));
def AC=solve(I,6,0,"nodisplay");  // solutions should not be displayed
// list of solutions is stored in AC as the list SOL (default name)
setring AC;
size(SOL);               // number of different solutions
SOL[5];                  // the 5th solution
// you must start with char. 0
setring A;
def AC1=solve(I,6,1,"nodisplay");
setring AC1;
size(SOL);               // number of different multiplicities
SOL[1][1][1];            // a solution with
SOL[1][2];               // multiplicity 1
SOL[2][1][1];            // a solution with
SOL[2][2];               // multiplicity 12
// the number of different solutions is equal to
size(SOL[1][1])+size(SOL[2][1]);
// the number of complex solutions (counted with multiplicities) is
size(SOL[1][1])*SOL[1][2]+size(SOL[2][1])*SOL[2][2];
tst_status(1);$
