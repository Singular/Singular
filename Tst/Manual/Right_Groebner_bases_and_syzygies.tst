LIB "tst.lib"; tst_init();
// ----- setting up the algebra:
LIB "ncalg.lib";
def A = makeUsl2();
setring A; A;
// ----- equivalently, you may use
// ring AA = 0,(e,f,h),dp;
// matrix D[3][3];
// D[1,2]=-h; D[1,3]=2*e; D[2,3]=-2*f;
// def A=nc_algebra(1,D); setring A;
option(redSB);
option(redTail);
matrix T;
// --- define a generating set
ideal   I = e2,f;
ideal  LI = std(I); // the left Groebner basis of I
LI;             // we see that I was not a Groebner basis
module LS = syz(I); // the left syzygy module of I
print(LS);
 // check: LS is a left syzygy, if T=0:
T  = transpose(LS)*transpose(I);
print(T);
// --- let us define the opposite algebra of A
def Aop = opposite(A);
setring Aop; Aop;         // see how Aop looks like
// --- we "oppose" (transfer) objects from A to Aop
ideal   Iop = oppose(A,I);
ideal  RIop = std(Iop);  // the left Groebner basis of Iop in Aop
module RSop = syz(Iop);  // the left syzygy module of Iop in Aop
module LSop = oppose(A,LS);
module RLS  = syz(transpose(LSop));
// RLS is the left syzygy of transposed LSop in Aop
// --- let us return to A and transfer (i.e. oppose)
// all the computed objects back
setring A;
ideal  RI = oppose(Aop,RIop); // the right Groebner basis of I
RI;              // it differs from the left Groebner basis LI
module RS = oppose(Aop,RSop); // the right syzygy module of I
print(RS);
 // check: RS is a right syzygy, if T=0:
T = matrix(I)*RS;
T;
module RLS;
RLS = transpose(oppose(Aop,RLS));
// RLS is the right syzygy of a left syzygy of I
// it is I itself ?
print(RLS);
tst_status(1);$
