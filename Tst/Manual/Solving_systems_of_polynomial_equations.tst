LIB "tst.lib"; tst_init();
LIB "solve.lib";
ring r=0,x(1..5),dp;
poly f0= x(1)^3+x(2)^2+x(3)^2+x(4)^2-x(5)^2;
poly f1= x(2)^3+x(1)^2+x(3)^2+x(4)^2-x(5)^2;
poly f2=x(3)^3+x(1)^2+x(2)^2+x(4)^2-x(5)^2;
poly f3=x(4)^2+x(1)^2+x(2)^2+x(3)^2-x(5)^2;
poly f4=x(5)^2+x(1)^2+x(2)^2+x(3)^2;
ideal i=f0,f1,f2,f3,f4;
ideal si=std(i);
//
// dimension of a solution set (here: 0) can be read from a Groebner bases
// (with respect to any global monomial ordering)
dim(si);
//
// the number of complex solutions (counted with multiplicities) is:
vdim(si);
//
// The given system has a multiple solution at the origin. We use facstd
// to compute equations for the non-zero solutions:
option(redSB);
ideal maxI=maxideal(1);
ideal j=sat(si,maxI);  // output is Groebner basis
vdim(j);                  // number of non-zero solutions (with mult's)
//
// We compute a triangular decomposition for the ideal I. This requires first
// the computation of a lexicographic Groebner basis (we use the FGLM
// conversion algorithm):
ring R=0,x(1..5),lp;
ideal j=fglm(r,j);
list L=triangMH(j);
size(L);                 // number of triangular components
L[1];                    // the first component
//
// We compute floating point approximations for the solutions (with 30 digits)
def S=triang_solve(L,30);
setring S;
size(rlist);             // number of different non-zero solutions
rlist[1];                // the first solution
//
// Alternatively, we could have applied directly the solve command:
setring r;
def T=solve(i,30,1,"nodisplay");  // compute all solutions with mult's
setring T;
size(SOL);               // number of different solutions
SOL[1][1]; SOL[1][2];    // first solution and its multiplicity
SOL[size(SOL)];          // solutions of highest multiplicity
//
// Or, we could remove the multiplicities first, by computing the
// radical:
setring r;
ideal k=std(radical(i));
vdim(k);                 // number of different complex solutions
def T1=solve(k,30,"nodisplay");  // compute all solutions with mult's
setring T1;
size(SOL);               // number of different solutions
SOL[1];
tst_status(1);$
