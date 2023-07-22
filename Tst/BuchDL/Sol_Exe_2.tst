LIB "tst.lib";
tst_init();


//======================  Exercise 2.1 =============================
ring R = 0, (x,y), dp;
poly f1, f2, f3 = x2+y2, x2y2, x3y-xy3;
if (not(defined(algDependent))) { LIB "algebra.lib"; }
def L = algDependent(ideal(f1,f2,f3));
L[1];
def S = L[2];
setring S;
ker;

setring R;
poly g, g1, g2 = x4+y4, x+y, xy;
L = algebra_containment(g,ideal(g1,g2),1);
def S2 = L[2];
setring S2;
check;

ring T = 0, x(1..3), dp;
qring Q = groebner(x(1)*x(2)*x(3)-1);
map phi = Q, x(2)*x(3), x(1)*x(3), x(1)*x(2);
is_bijective(phi,Q);
//-> 1


kill R,Q,S,S2,T,L;
//======================  Exercise 2.2 =============================
ring R = 0, (w,x,y,z), dp;
ideal I = y-x2, z-x3;
ideal SI = groebner(I);
ideal Ih = homog(SI,w);          // generators are homogenized
Ih;

ideal J = homog(I,w);            // generators are homogenized
size(reduce(J,groebner(Ih),1));  // J is contained in Ih
size(reduce(Ih,groebner(J),1));  // Ih is not contained in J
if (not(defined(sat))){ LIB "elim.lib"; }
ideal Iinf = sat(J,Ih);
Iinf;


kill R;
//======================  Exercise 2.3 =============================
ring S = 0, (y,x), dp;
ideal I = maxideal(3),maxideal(2),x,y,1;
ring R = 0, (x,y,a(1..10)), (dp(2),dp(10));
ideal I = imap(S,I);
matrix A[10][1] = a(1..10);
poly f = (matrix(I)*A)[1,1];
ideal J = f, diff(f,x), diff(f,y);
J = groebner(J);
// check for generators for J that do not depend on x,y
ideal JJ;
for (int i=1; i<=size(J); i++)
{
  if (J[i][1]<y) { JJ = JJ,J[i]; }
}
JJ = simplify(JJ,2);             // erase zero generators
size(JJ);
//-> 1
homog(JJ);
//-> 1
poly D = JJ[1];
deg(D); size(D);
//-> 12
//-> 2040


kill i,R,S;
//======================  Exercise 2.4 =============================
//======== Procedures are stored in the library file sol.lib =======
//==================================================================
LIB "sol.lib";
example ideal_intersect;
example ideal_quotient;
example saturate;

tst_status(1);$

