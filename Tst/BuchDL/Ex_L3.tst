LIB "tst.lib";
tst_init();


2+3+4;
//->   9
2+3; 3+5;
//->   5
//->   8

1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1
+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1;
//->   42

1/3 + 1/5;
//->      ? no ring active
//->      ? error occurred in STDIN line ...: `1/3 + 1/5;`

ring R = 0, (x,y), dp;
ring R1 = 0, x(1..7), dp;
ring R2 = (0,i), (x,y), dp;
minpoly = i^2+1;
ring R3 = (2,a), (x,y), dp;
minpoly = a^3+a+1;
ring R3prime = (2^3,a), (x,y), dp;
ring R4 = (32003,s,t), (x,y), dp;

//======================  Example 3.2 =============================
ring R5 = 0, (w,x,y,z), dp;
matrix m[2][3] = w,x,y,x,y,z;
ideal I = minor(m,2);
qring Q = groebner(I);
basering;
//->   //   characteristic : 0
//->   //   number of vars : 4
//->   //        block   1 : ordering dp
//->   //                  : names    w x y z
//->   //        block   2 : ordering C
//->   // quotient ring from ideal
//->   _[1]=y2-xz
//->   _[2]=xy-wz
//->   _[3]=x2-wy

setring R2;
1/3+1/5;
//->   8/15
setring R3;
1/3+1/5;
//->   0

varstr(R3);
//->   x,y
poly f = x+y;
f;
//->   x+y
ring S3 = (2^3,a), (y,x), dp;
poly f = x+y;
f;
//->   y+x


kill R,R1,R2,R3,R3prime,R4,R5,Q,S3;
//====================== new Session ==============================
ring R = 0, x(1..7), (dp(3),wp(2,1),dp);
basering;
//->   //   characteristic : 0
//->   //   number of vars : 7
//->   //        block   1 : ordering dp
//->   //                  : names    x(1) x(2) x(3)
//->   //        block   2 : ordering wp
//->   //                  : names    x(4) x(5)
//->   //                  : weights     2    1
//->   //        block   3 : ordering dp
//->   //                  : names    x(6) x(7)
//->   //        block   4 : ordering C

kill R;
ring R = 0, (x,y,z), M(1,1,1, 0,0,-1, 0,-1,0);

kill R;
ring R = 0, (x,y,z,w), (a(1,0,0,1),dp);

kill R;
//====================== Example 3.8 (new Session) ================
ring R = 0, (x,y), (c,dp);

kill R;
//====================== Remark 3.9 (new Session) =================
ring R = 0, (x,y,e(1..3)), wp(1,1,-1,-2,-4);
ideal I = e(1..3);
qring Q = groebner(I^2);


kill Q,R;
//======================  new Session =============================
ring R = 0, (x,y), dp;
int i = 1;              // object of ring independent type int
poly f = x;             // object of ring dependent type poly
ring S = 0, (x,y), dp;  // active ring is changed
poly g = y;
f;
//->      ? `f` is undefined
//->      ? error occurred in STDIN line 6: `f;`

listvar();
//->   // S                    [0]  *ring
//->   //      g                    [0]  poly
//->   // i                    [0]  int 1
//->   // R                    [0]  ring


kill S,i,R;
//======================  Example 3.10 (new Session) ==============
ring R = 0, (x,y), dp;
poly f = x2+y;
ring S = 0, (a,b,c), dp;
map F = R, a-b, c;  // map F: R->S, sending x to a-b, y to c
poly g = F(f);      // apply the map
g;
//->   a2-2ab+b2+c
ring S1 = 2, (a,b,c), lp;
qring Q = std(a^2);
map F1 = R, a-b, c; // target ring is qring, with another
                    // characteristic and monomial order
poly g=F1(f);
g;                  // polynomial is not yet reduced
//->   a2+b2+c
reduce(g,std(0));
//->   b2+c


ring R1 = 0, (a,b,c,x,y,z), dp;
fetch(R,f);         // fetch preserves order of variables
//->   a2+b
imap(R,f);          // imap preserves names of variables
//->   x2+y
fetch(Q,g);
//->   a2+b2+c


kill R,S,S1,R1,Q;
//======================  new Session =============================
ring R = 0, (x,y,z), dp;
ideal I = x2-y, y4-z2;


kill R;
//======================  new Session =============================
ring R = 0, (x,y,z), dp;
poly f = x2-y;
poly g = y4-z2;
ideal I = f,g;
vector v = [f,0,0,g,0];

kill v; // ---- Alternatively:
vector v = f*gen(1)+g*gen(4);

kill R;
ring R = 0, (x,y,z), (c,dp);
vector v = [x,y]+[x2,1,z3,0];
v;
//->   [x2+x,y+1,z3]

ring S = 0, (x,y,z), (dp,c);
vector v = fetch(R,v);
v;
//->   z3*gen(3)+x2*gen(1)+x*gen(1)+y*gen(2)+gen(2)

print(v);
//->   [x2+x,y+1,z3]


kill S,R;
//======================  new Session =============================
ring R = 0, (x,y), dp;
module I = [x2,-y,y,-y,0], [0,0,y], [y,x];
print(I);
//->   x2,0,y,
//->   -y,0,x,
//->   y, y,0,
//->   -y,0,0

matrix MI[4][3] =  x2, 0, y,
                   -y, 0, x,
                    y, y, 0,
                   -y, 0, 0;

kill MI; // ---- Alternatively:
matrix MI = I;


kill R;
//====================== Section 3.4 (new Session) =================
ring R = 0, (w,x,y,z), dp;
module I = [xz,0,-w,-1,0], [-yz2,y2, 0,-w,0], [y2z,0,-z2,0,-x],
           [y3,0,-yz,-x,0], [-z3,yz,0,0,-w], [-yz2,y2,0,-w,0],
           [0,0,-wy2+xz2,-y2,x2];
print(I);
//->   xz,-yz2,y2z,y3, -z3,-yz2,0,
//->   0, y2,  0,  0,  yz, y2,  0,
//->   -w,0,   -z2,-yz,0,  0,   -wy2+xz2,
//->   -1,-w,  0,  -x, 0,  -w,  -y2,
//->   0, 0,   -x, 0,  -w, 0,   x2

attrib(I,"isHomog");     // no attribute => empty output
homog(I);
//->   1
attrib(I,"isHomog");
//->   0,1,1,2,2

print(betti(I,0),"betti");
//->              0     1
//->   ------------------
//->       0:     1     -
//->       1:     2     1
//->       2:     2     5
//->       3:     -     1
//->   ------------------
//->   total:     5     7

intvec DV =  2,3,3,4,4;
attrib(I,"isHomog",DV);

attrib(I,"isHomog");
//->   2,3,3,4,4
print(betti(I,0),"betti");
//->              0     1
//->   ------------------
//->       2:     1     -
//->       3:     2     1
//->       4:     2     5
//->       5:     -     1
//->   ------------------
//->   total:     5     7

intmat BI = betti(I,0);
int d = attrib(BI,"rowShift");
d;
//->   2


kill R,DV,BI,d;
//====================== Section 3.5 (new Session) =================
ring R = 0, (x,y,z), lp;
ideal  I = 2y+z,3x-y;
std(I);
//->   _[1]=2y+z
//->   _[2]=3x-y
option(redSB);
ideal G = std(I);
G;
//->   G[1]=2y+z
//->   G[2]=6x+z
G = simplify(G,1);
G;
//->   G[1]=y+1/2z
//->   G[2]=x+1/6z


kill R;
//====================== Example 3.13 (new Session) ===============
ring R = 0, (x,y,z), dp;
ideal I = 3x3y+x3+xy3+y2z2, 2x3z-xy-xz3-y4-z2, 2x2yz-2xy2+xz2-y4;
option(redSB);          // force computation of reduced GBs
int aa = timer;
ideal SI = std(I);
size(SI);  dim(SI);

ring S = 0, (x,y,z), lp;
aa = timer;
ideal J = fglm(R,SI);

size(J);                  // number of generators
//->   8
size(string(J)) div 68;   // number of lines with 68 characters
                          // needed to display J:
//->   631
deg(J[1..size(J)]);       // degrees of the generators
//->   35 34 34 34 34 34 34 34
leadmonom(J[1..size(J)]); // generators for L(I) w.r.t. lp
//->   z35 yz6 y2z4 y3z2 y5 xz2 xy x3
leadcoef(J[8]);           // leading coefficient of 8th generator
//->   6440093631623773985969509841859276602512807348986590906348582267651806942677443883093109641441627364249598438582596862938314965556548533870597328962260825040847335705757819599104

ideal I = fetch(R,I);
// I = std(I);
//->   error: no more memory


kill S,aa,R;
//====================== Example 3.14 (new Session) ===============
LIB "grwalk.lib";
ring S = 0, (x,y,z), lp;
ideal I = 3x3y+x3+xy3+y2z2, 2x3z-xy-xz3-y4-z2, 2x2yz-2xy2+xz2-y4;
option(redSB);          // force computation of reduced GBs
int aa = timer;
ideal J = fwalk(I);
size(J), dim(J);

kill S,aa;
//====================== Example 3.16 (new Session) ===============
ring S = 0, (x,y,z), lp;
ideal I = 3x3y+x3+xy3+y2z2, 2x3z-xy-xz3-y4-z2, 2x2yz-2xy2+xz2-y4;
option(redSB);
ring Rhom = 0, (x,y,z,t), dp;
ideal I = imap(S,I);
ideal Ih = homog(I,t);   // generators of I are homogenized
int aa = timer;
Ih = std(Ih);
intvec H = hilb(Ih,1);
ring Shom = 0, (x,y,z,t), lp;
ideal Ih = imap(Rhom,Ih);
Ih = std(Ih,H);
Ih = subst(Ih,t,1);
setring S;
ideal J = imap(Shom,Ih);
size(J);
//->   102

J = interred(J);
//->   3
size(J);
//->   8
dim(J);


kill S,Rhom,aa,H,Shom;
//====================== Remark 3.18 (new Session) ===============
ring R = (32003,a,b,c,d), (t,u,v,w,x,y,z), dp;
ideal I = -cw+bx, ct+2au-2bu-2cv-(ad+bd),
          -2tx+4wy+4xz+ct-2aw-2dw-2by-2cz+(ab+bd),
          t*(z-x)+(a-b+d)*(y-w)+c*(x-z), -tw+a*(t-x)+dx,
          -2tv+ct-2du+(ad+bd), ct2-(b2-ab+c2)*t-(acd-cd2);
int aa = timer;
ideal SI = slimgb(I);
size(SI), dim(SI);

SI = std(I);
size(SI), dim(SI);



kill R,aa;
//====================== Example 3.20 (new Session) ===============
ring S = 0, (x,y,z), lp;
ideal I = 3x3y+x3+xy3+y2z2, 2x3z-xy-xz3-y4-z2, 2x2yz-2xy2+xz2-y4;
option(redSB);
option(prot);
int aa = timer;
ideal J = groebner(I);
//->   std in (0),(x,y,z,@t),(dp,C)
//->   [255:1]4(2)sss5s6s7(3)s(4)s(5)s(6)s8(8)s(9)-ss(11)s(12)---9-s(9)-s(
//->   10)--s--10-s(8)s(9)-s---11------
//->   product criterion:9 chain criterion:124
//->   std with hilb in  (0),(x,y,z,@t),(lp(3),C)
//->   [255:1]4(2)sss5ss6s(3)s(5)s7(6)s(7)s(9)s(11)s(13)-s(14)s8(16)s(17)s
//->   (19)s(21)s(23)s(25)s(27)s(28)-s(29)--shhhhh9(24)s(26)s(28)s(30)s(32
//->   )s(33)s(35)s(37)s(39)s(41)shhhhhhhhhhhhhhh10(28)ss(29)s(30)s(32)s(3
//->   4)s(35)s(37)s(39)s(41)s(43)shhhhhhhhhhhhhhhhhh11(26)s(28)s(30)s(32)
//->   s(34)s(35)shhhhhhhhhhhhhhhhhhhhh12(16)s(18)s(20)s(22)s(24)shhhhhhhh
//->   hhhh13(14)s(15)s(17)s(19)s(21)shhhhhhhhhh14(13)s(15)s(17)s(19)shhhh
//->   hhhhhh15(10)s(12)s(14)shhhhhhhh16(8)s(10)s(12)shhhhhh17(8)s(10)s(12
//->   )shhhhhh18(8)s(9)s(11)shhhhhh19(7)s(9)shhhhhh20(5)s(7)shhhh21(5)s(7
//->   )shhhh22(5)s(7)shhhh23(5)s(7)shhhh24(5)s(6)shhhh25(4)shhhh26(2)shh2
//->   7shh28shh29shh30shh31shh32shh33shh34shh35shh36shh37shh38shhhh
//->   product criterion:27 chain criterion:4846
//->   hilbert series criterion:175
//->   dehomogenization
//->   imap to original ring
//->   simplification
//->   interreduction
size(J);
//->   8


kill S,aa;
option(noprot);
//====================== Example 3.21 (new Session) ===============
ring R = 0, (x,y), dp;
ideal I = 4x2y2+3x, y3+2xy, 7x3+6y;
std(I);
//->   _[1]=y
//->   _[2]=x

ideal J = x;
matrix A = lift(I,J);
A;
//->   A[1,1]=-3670016/18809541x2y+9604/6269847xy2-134217728/131666787y3-128/63xy-100352/6269847y2-458752/6269847y+1/3
//->   A[2,1]=536870912/131666787x2y2+401408/6269847x2y+1835008/6269847x2-4194304/6269847xy+10976/2089949y2+64/21x+50176/2089949y+229376/2089949
//->   A[3,1]=2097152/18809541xy3-5488/6269847y4-25088/6269847y3-114688/6269847y2
matrix(I)*A;
//->   _[1,1]=x


kill R;
//====================== Example 3.22 (new Session) ===============
ring R = 0, (x,y), dp;
ideal I = x7+x5y2, y4-xy7;
poly f1, f2 = x6y7+x3y5, x6y7+x7y2;
ideal GI = groebner(I);
reduce(f1,GI,1);     // see Example 1.39 for reduce
//->   y5-y4
reduce(f2,GI,1);
//->   0
lift(I,f1);
//->      ? 2nd module lies not in the first
//->      ? error occurred in STDIN line 8: `lift(I,f1);  `
matrix C = lift(I,f2);
C;
//->   C[1,1]=x4y22-x2y24-x3y19+xy21+y2
//->   C[2,1]=x10y15-x6y19-x5
f2 - C[1,1]*I[1] - C[2,1]*I[2];  // check (result must be 0)
//->   0

ideal J1 = f1, f2;
ideal J2 = f2, x5y9+x6y4;
reduce(J1,GI,1);     // normal form for the generators of J1
//->   _[1]=y5-y4
//->   _[2]=0
size(reduce(J2,GI,1));
//->   0


kill R;
//====================== Example 3.23 (new Session) ===============
ring R = 0, (x,y), dp;
ideal I = maxideal(3);  // the ideal <x,y>^3
poly f1, f2 = x, 1-x;
ring S = 0, (x,y,t), dp;
ideal I = imap(R,I);
poly f1 = imap(R,f1);
ideal Jf1 = I, t*f1-1;
Jf1 = std(Jf1);
reduce(1,Jf1,1);    // result is 0 iff f1 is in radical(I)
//->   0
poly f2 = imap(R,f2);
ideal Jf2 = I, t*f2-1;
Jf2 = std(Jf2);
reduce(1,Jf2,1);
//->   1

setring R; // ---- Alternatively ---------
if (not(defined(rad_con))){ LIB "polylib.lib"; }
rad_con(f1,I);      // result is 1 iff f is in radical(I)
//->   1
rad_con(f2,I);
//->   0


kill R,S;
//====================== Example 3.24 (new Session) ===============
ring R = 0, (a,b,c,d,e,f,g,t,u,v,w,y,z), dp;
ideal I = z2+e2-1, g2+w2+a2-1, t2+u2+b2-1, f2+v2+c2-1, y2+d2-1,
          zw+ea, gt+wu+ab, tf+uv+bc, fy+cd, a+b+c+d+e, f+g+t+y+1,
          u+v+w+z-1;
ring Rhom = 0, (a,b,c,d,e,f,g,t,u,v,w,y,z,h), dp;
ideal I = imap(R,I);
ideal J = homog(I,h);    // homogenize the given generators
int aa = timer;
ideal L = std(J);
intvec H = hilb(L,1);    // assign Hilbert series
ideal K = eliminate(J,abcdefgtuvw,H);
K = subst(K,h,1);        // dehomogenize
size(K);
//->   1
K[1];                    // the equation
//->   790272y16z16-3612672y16z15+3612672y15z16-6530048y16z14-6006784y15z15
//->   -6530048y14z16+41607168y16z13-56159232y15z14+[...]


kill R,Rhom,aa,H;
//====================== Example 3.27 (new Session) ===============
ring R = 0, x(1..3), dp;
poly f1 = x(1)^6*x(3)^2-x(2)^6*x(3)^2;
poly f2,f3,f4 = x(1)^3-x(2)^3, x(1)^3+x(2)^3, x(3)^3;
ring S = 0, y(1..4), dp;
setring R;
ideal zero;              // the zero ideal
map phi = S,f1,f2,f3,f4;
setring S;
preimage(R,phi,zero);    // the kernel of phi
//->   _[1]=y(2)^3*y(3)^3*y(4)^2-y(1)^3

setring R; //--- Alternatively:
kill S;
if (not(defined(algDependent))){ LIB "algebra.lib"; }
list L = algDependent(ideal(f1,f2,f3,f4));
L[1];            // first entry of L is 1 iff the polynomials are
                 // algebraically dependent
//->   1
def S = L[2];    // second entry of L is a ring which contains
                 // an ideal ker defining the algebraic relation
setring S;
ker;
//->   ker[1]=y(2)^3*y(3)^3*y(4)^2-y(1)^3


kill R,S,L;
//====================== Example 3.28 (new Session) ===============
ring R = 0, x(1..3), dp;
poly f = x(1)^6*x(2)^6-x(1)^6*x(3)^6;
poly f1 = x(1)^3*x(2)^3-x(1)^3*x(3)^3;
poly f2 = x(1)^3*x(2)^3+x(1)^3*x(3)^3;
ring S = 0, (x(1..3),y(1..2)), (dp(3),dp(2));
ideal J = imap(R,f1)-y(1), imap(R,f2)-y(2);
ideal G = groebner(J);
reduce(imap(R,f),G);
//->   y(1)*y(2)

setring R; kill S; //--- Alternatively:
if (not(defined(algebra_containment))){ LIB "algebra.lib"; }
algebra_containment(f,ideal(f1,f2));
//->   // y(1)*y(2)
//->   1
def L = algebra_containment(f,ideal(f1,f2),1);
def S = L[2];
setring S;
check;              // polynomial defining the algebraic relation
//->   y(1)*y(2)


kill R,S,L;
//====================== Example 3.29 (new Session) ===============
ring C = 0, (a,b,c,x,y,z), (dp(3),dp);
ideal J = c-b3, 2a+b6-x, 7b-a2-y, c2-z;
option(redSB);
simplify(groebner(J),1);    // the reduced Groebner basis for J
//->   _[1]=x12-12x11z+66x10z2-220x9z3+495x8z4-792x7z5+[...]
//->   _[2]=c-1/21952x6+3/10976x5z-15/21952x4z2+[...]
//->   _[3]=b-1/28x2+1/14xz-1/28z2-1/7y
//->   _[4]=a-1/2x+1/2z

if (not(defined(is_surjective))){ LIB "algebra.lib"; }
ring B = 0, (x,y,z), dp;
ring A = 0, (a,b,c), dp;
qring Q = groebner(c-b3);   // quotient ring
map psi = B, 2a+b6, 7b-a2, c2;
is_surjective(psi);
//->   1


kill A,B,C,Q;
//====================== Example 3.30 (new Session) ===============
ring R = 0, (w,x,y,z), dp;
poly f1, f2, f3 = y2-xz, xy-wz, x2z-wyz;
ideal I = f1, f2, f3;
module phi2 = syz(I);
print(phi2);
//->   x, wz,
//->   -y,-xz,
//->   1, y
size(syz(phi2));  // we check that there are no higher syzygies
//->   0

resolution FI = nres(I,0);
typeof(FI[1]);        // 'typeof' displays type of given object
//->   ideal
print(FI[1]);
//->   y2-xz,
//->   xy-wz
//->   x2z-wyz
typeof(FI[2]);
//->   module
print(FI[2]);
//->   x, wz,
//->   -y,-xz,
//->   1, y

print(betti(FI),"betti");
//->              0     1     2
//->   ------------------------
//->       0:     1     -     -
//->       1:     -     2     -
//->       2:     -     -     1
//->   ------------------------
//->   total:     1     2     1

print(betti(FI,0),"betti");
//->              0     1     2
//->   ------------------------
//->       0:     1     -     -
//->       1:     -     2     1
//->       2:     -     1     1
//->   ------------------------
//->   total:     1     3     2


kill R;
//====================== Example 3.35 (new Session) ===============
ring R = 0, (w,x,y,z), dp;
module I = [xz,0,-w,-1,0], [-yz2,y2, 0,-w,0], [y2z,0,-z2,0,-x],
           [y3,0,-yz,-x,0], [-z3,yz,0,0,-w], [-yz2,y2,0,-w,0],
           [0,0,-wy2+xz2,-y2,x2];
print(I);
//->   xz,-yz2,y2z,y3, -z3,-yz2,0,
//->   0, y2,  0,  0,  yz, y2,  0,
//->   -w,0,   -z2,-yz,0,  0,   -wy2+xz2,
//->   -1,-w,  0,  -x, 0,  -w,  -y2,
//->   0, 0,   -x, 0,  -w, 0,   x2
homog(I);
//->   1
attrib(I,"isHomog");
//->   0,1,1,2,2
resolution FInres = nres(I,0);
print(betti(FInres,0),"betti");
//->              0     1     2
//->   ------------------------
//->       0:     1     -     -
//->       1:     2     1     1
//->       2:     2     5     1
//->       3:     -     1     1
//->   ------------------------
//->   total:     5     7     3
print(FInres[1]);   // the given generators
//->   xz,-yz2,y2z,y3, -z3,-yz2,0,
//->   0, y2,  0,  0,  yz, y2,  0,
//->   -w,0,   -z2,-yz,0,  0,   -wy2+xz2,
//->   -1,-w,  0,  -x, 0,  -w,  -y2,
//->   0, 0,   -x, 0,  -w, 0,   x2
print(FInres[2]);   // display syzygies on the given generators
//->   0, y2,0,
//->   -1,0, xz,
//->   0, -x,wy,
//->   0, 0, -wz,
//->   0, 0, -xy,
//->   1, 0, 0,
//->   0, -1,0
size((FInres[3]));
//->   0
resolution FImres = mres(I,0);
print(betti(FImres,0),"betti");
//->              0     1     2
//->   ------------------------
//->       0:     1     -     -
//->       1:     2     1     -
//->       2:     2     4     -
//->       3:     -     -     1
//->   ------------------------
//->   total:     5     5     1
print(FImres[1]);   // the new generators
//->   xz,z3, yz2,y2z,y3,
//->   0, -yz,-y2,0,  0,
//->   -w,0,  0,  -z2,-yz,
//->   -1,0,  w,  0,  -x,
//->   0, w,  0,  -x, 0
print(FImres[2]);   // display syzygies on the new generators
//->   0,
//->   xy,
//->   -xz,
//->   wy,
//->   -wz
module PI = prune(I);
print(betti(PI,0),"betti");
//->              0     1
//->   ------------------
//->       0:     1     -
//->       1:     2     -
//->       2:     1     5
//->       3:     -     1
//->   ------------------
//->   total:     4     6
print(PI);
//->   wxz+yz2,-y2z,-y3+x2z,z3, wxz+yz2,xy2z,
//->   -y2,    0,   0,      -yz,-y2,    0,
//->   -w2,    z2,  -wx+yz, 0,  -w2,    -xz2,
//->   0,      x,   0,      w,  0,      -x2
resolution FPImres = mres(PI,0);
print(betti(FPImres,0),"betti");
//->              0     1     2
//->   ------------------------
//->       0:     1     -     -
//->       1:     2     -     -
//->       2:     1     4     -
//->       3:     -     -     1
//->   ------------------------
//->   total:     4     4     1
print(FPImres[1]);
//->   z3, y2z,wxz+yz2,y3-x2z,
//->   -yz,0,  -y2,    0,
//->   0,  -z2,-w2,    wx-yz,
//->   w,  -x, 0,      0
print(FPImres[2]);
//->   xy,
//->   wy,
//->   -xz,
//->   -wz
resolution FPIsres = sres(PI,0);
//->      ? ideal not a standardbasis
//->      ? error occurred in STDIN line 27:
//->        `resolution FPIsres = sres(PI,0);`
resolution FPIsres = sres(groebner(PI),0);
print(betti(FPIsres,0),"betti");
//->              0     1     2     3     4
//->   ------------------------------------
//->       0:     1     -     -     -     -
//->       1:     2     -     -     -     -
//->       2:     1     4     1     -     -
//->       3:     -     1     5     1     -
//->       4:     -     4     2     3     1
//->       5:     -     1     3     1     -
//->   ------------------------------------
//->   total:     4    10    11     5     1

resolution FInresmin = minres(FInres);
print(betti(FInresmin,0),"betti");
//->              0     1     2
//->   ------------------------
//->       0:     1     -     -
//->       1:     2     1     -
//->       2:     2     4     -
//->       3:     -     -     1
//->   ------------------------
//->   total:     5     5     1
resolution FPIsresmin = minres(FPIsres);
print(betti(FPIsresmin,0),"betti");
//->              0     1     2
//->   ------------------------
//->       0:     1     -     -
//->       1:     2     -     -
//->       2:     1     4     -
//->       3:     -     -     1
//->   ------------------------
//->   total:     4     4     1


kill R;
listvar();
//====================== Example 3.40(1) (new Session) ===============
ring D3 = 0, (x(1..3),d(1..3)), dp;
int i,j;
matrix C[6][6];
for (i=1; i<=6; i++) { for (j=i+1; j<=6; j++) { C[i,j] = 1; } }
matrix H[6][6];
H[1,4] = 1;  H[2,5] = 1;  H[3,6] = 1;
def @S=nc_algebra(C,H); setring @S;

kill D3;   // ---- Alternatively:
if (not(defined(Exterior))){ LIB "nctools.lib"; }
ring D3 = 0, (x(1..3),d(1..3)), dp;
def @S3=Weyl(); setring @S3;

basering;
//->   //   characteristic : 0
//->   //   number of vars : 6
//->   //        block   1 : ordering dp
//->   //                  : names    x(1) x(2) x(3) d(1) d(2) d(3)
//->   //        block   2 : ordering C
//->   //   noncommutative relations:
//->   //    d(1)x(1)=x(1)*d(1)+1
//->   //    d(2)x(2)=x(2)*d(2)+1
//->   //    d(3)x(3)=x(3)*d(3)+1

//================= Example 3.44 (continued Session) ===============
ideal I = x(1)^2*d(2)^2+x(2)^2*d(3)^2, x(1)*d(2)+x(3);
option(redSB);
ideal LSI = std(I);
LSI;
//->   LSI[1]=x(1)*d(2)+x(3)
//->   LSI[2]=x(3)^2
//->   LSI[3]=x(2)*x(3)-x(1)
//->   LSI[4]=x(1)*x(3)
//->   LSI[5]=x(2)^2
//->   LSI[6]=x(1)*x(2)
//->   LSI[7]=x(1)^2
def D3_opp = opposite(@S3);
setring D3_opp;   // active ring is the opposite algebra of @S3
basering;
//->   //   characteristic : 0
//->   //   number of vars : 6
//->   //        block   1 : ordering a
//->   //                  : names    D(3) D(2) D(1) X(3) X(2) X(1)
//->   //                  : weights     1    1    1    1    1    1
//->   //        block   2 : ordering ls
//->   //                  : names    D(3) D(2) D(1) X(3) X(2) X(1)
//->   //        block   3 : ordering C
//->   //   noncommutative relations:
//->   //    X(3)D(3)=D(3)*X(3)+1
//->   //    X(2)D(2)=D(2)*X(2)+1
//->   //    X(1)D(1)=D(1)*X(1)+1
ideal I = oppose(@S3,I);   // map I to opposite algebra
ideal RSI_opp = std(I);
setring @S3;
ideal RSI = oppose(D3_opp,RSI_opp);
RSI;
//->   RSI[1]=x(1)*d(2)+x(3)
//->   RSI[2]=x(3)^2
//->   RSI[3]=x(2)*x(3)+x(1)
//->   RSI[4]=x(1)*x(3)
//->   RSI[5]=x(2)^2
//->   RSI[6]=x(1)*x(2)
//->   RSI[7]=x(1)^2
size(reduce(RSI,LSI));
//->   1
ideal SI = twostd(I);
SI;
//->   SI[1]=1


kill i,j,D3,D3_opp,@S3;
//====================== Example 3.40(2) (new Session) ===============
ring R = 0, x(1..3), dp;
int i,j;
matrix C[3][3];
for (i=1; i<=3; i++) { for (j=i+1; j<=3; j++) { C[i,j] = -1; } }
matrix H[3][3];
def @SS=nc_algebra(C,H); setring @SS;
ideal Q = x(1)^2, x(2)^2, x(3)^2;
Q = twostd(Q);     // compute two-sided Groebner basis
qring E3 = Q;

kill E3,R;   // ---- Alternatively:
if (not(defined(Exterior))){ LIB "nctools.lib"; }
ring R = 0, x(1..3), dp;
def E3 = Exterior();
setring E3;
basering;
//->   //   characteristic : 0
//->   //   number of vars : 3
//->   //        block   1 : ordering dp
//->   //                  : names    x(1) x(2) x(3)
//->   //        block   2 : ordering C
//->   //   noncommutative relations:
//->   //    x(2)x(1)=-x(1)*x(2)
//->   //    x(3)x(1)=-x(1)*x(3)
//->   //    x(3)x(2)=-x(2)*x(3)
//->   // quotient ring from ideal
//->   _[1]=x(3)^2
//->   _[2]=x(2)^2
//->   _[3]=x(1)^2

//================= Example 3.46 (continued Session) ===============
ideal I = maxideal(1);
def rI = mres(I,0);
//->   // ** full resolution in a qring may be infinite,
//->   //    setting max length to 5
print(betti(rI),"betti");
//->              0     1     2     3     4     5
//->   ------------------------------------------
//->       0:     1     3     6    10    15    21
//->   ------------------------------------------
//->   total:     1     3     6    10    15    21
print(rI[1],"");
//->   x(3),x(2),x(1)
print(rI[2]);
//->   x(3),x(2),0,   x(1),0,   0,
//->   0,   x(3),x(2),0,   x(1),0,
//->   0,   0,   0,   x(3),x(2),x(1)


kill i,j,E3,R;
//====================== Remark 3.52 (new Session) ===============
ring R = (0,a), (x(1..3),y(1..2),z(1..2)), (dp(3),wp(2,5),lp);
minpoly = a^2+1;
qring Q = std(y(1)^2-x(1));
list L = ringlist(Q);
size(L);
//->   4
L[1];
//->   [1]:
//->      0
//->   [2]:
//->      [1]:
//->         a
//->   [3]:
//->      [1]:
//->         [1]:
//->            lp
//->         [2]:
//->            1
//->   [4]:
//->      _[1]=(a^2+1)
L[2][7];
//->   z(2)
L[3];
//->   [1]:
//->      [1]:
//->         dp
//->      [2]:
//->         1,1,1
//->   [2]:
//->      [1]:
//->         wp
//->      [2]:
//->         2,5
//->   [3]:
//->      [1]:
//->         lp
//->      [2]:
//->         1,1
//->   [4]:
//->      [1]:
//->         C
//->      [2]:
//->         0
L[4];
//->   _[1]=x(1)-y(1)^2
L[1][2][1] = "b";            // new name for the parameter
L[2][8] = "w";               // append a new variable with name w
L[3][3][2] = intvec(1,1,1);  // raise the size of the third block
                             // of the monomial order
def S = ring(L);
setring S;
basering;
//->   //   characteristic : 0
//->   //   1 parameter    : b
//->   //   minpoly        : (b^2+1)
//->   //   number of vars : 8
//->   //        block   1 : ordering dp
//->   //                  : names    x(1) x(2) x(3)
//->   //        block   2 : ordering wp
//->   //                  : names    y(1) y(2)
//->   //                  : weights     2    5
//->   //        block   3 : ordering lp
//->   //                  : names    z(1) z(2) w
//->   //        block   4 : ordering C
//->   // quotient ring from ideal
//->   _[1]=x(1)-y(1)^2



tst_status(1);$

