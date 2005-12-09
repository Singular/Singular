LIB "tst.lib";
tst_init();


//======================  Example 6.8 =============================
option(redSB);
ring R = 0, x(1..5), lp;
poly f1 = x(1)^2+x(1)+2*x(2)*x(5)+2*x(3)*x(4);
poly f2 = 2*x(1)*x(2)+x(2)+2*x(3)*x(5)+x(4)^2;
poly f3 = 2*x(1)*x(3)+x(2)^2+x(3)+2*x(4)*x(5);
poly f4 = 2*x(1)*x(4)+2*x(2)*x(3)+x(4)+x(5)^2;
poly f5 = 2*x(1)*x(5)+2*x(2)*x(4)+x(3)^2+x(5);
ideal I = f1^2,f2^2,f3,f4,f5;
list L = facstd(I);
size(L);     // number of sets in the decomposition
//->   12
L[10];
//->   _[1]=5*x(5)-1
//->   _[2]=5*x(4)-1
//->   _[3]=5*x(3)-1
//->   _[4]=5*x(2)-1
//->   _[5]=5*x(1)+4
list L2 = facstd(I,5*x(5)-1);
size(L2);
//->   11
ring R1 = 0, x(1..5), dp;
ideal I = imap(R,I);

// fix CL: hier hat SINGULAR ein bug!!!! (manchmal voellig falsches Ergebnis)
size(facstd(I));
//->   1
size(facstd(I,5*x(5)-1));
//->   1


//==============  Example 6.13 (continued session)  ========================
if (not(defined(triangMH))){ LIB "triang.lib"; }
setring R;
ideal G = groebner(I);  // option(redSB) is already set
list T = triangMH(G); T;
//->   [1]:
//->      _[1]=x(5)
//->      _[2]=x(4)
//->      _[3]=x(3)
//->      _[4]=x(2)^2
//->      _[5]=x(1)^4+2*x(1)^3+x(1)^2
//->   [2]:
//->      _[1]=9765625*x(5)^10-1
//->      _[2]=x(4)-15625*x(5)^7
//->      _[3]=x(3)-25*x(5)^3
//->      _[4]=x(2)^2-781250*x(2)*x(5)^9+15625*x(5)^8
//->      _[5]=2*x(1)+31250*x(2)*x(5)^6+625*x(5)^5+1
//->   [3]:
//->      _[1]=95367431640625*x(5)^20-1201171875*x(5)^10+1
//->      _[2]=11*x(4)^2-1281738281250*x(4)*x(5)^17[...]
//->      _[3]=11*x(3)+152587890625*x(4)*x(5)^16-1906250*x(4)*x(5)^6[...]
//->      _[4]=22*x(2)+275*x(4)*x(5)^2+16021728515625*x(5)^19[...]
//->      _[5]=22*x(1)+3814697265625*x(4)*x(5)^18-47656250*x(4)*x(5)^8[...]


//==============  Example 6.14 (continued session)  ========================
if (not(defined(triang_solve))){ LIB "solve.lib"; }
triang_solve(T,20);      // 20 digits should be displayed
//->   // ** Laguerre: Too many iterations!
//->   // ** rootContainer::solver: No roots found!
LIB "primdec.lib";
list TR;
int k,j;
for (k=1; k<=size(T); k++) 
  { TR = TR + triangMH(std(zeroRad(T[k]))); }
triang_solve(TR,20);
//->   // ** Laguerre: Too many iterations!
//->   // ** rootContainer::solver: No roots found!
for (k=1; k<=size(TR); k++) { print(TR[k][1]); }
//->   x(5)
//->   9765625*x(5)^10-1
//->   95367431640625*x(5)^20-1201171875*x(5)^10+1
list TS = TR[1];
ideal J,JJ,LL;
for (k=2; k<=size(TR); k++)
{  J = TR[k];
   LL = factorize(J[1],2)[1]; // returns nonconstant factors only
   for (j=1; j<=size(LL); j++)
   {  JJ = LL[j],J[2..size(J)];
      TS = TS+list(JJ);  }
}
size(TS);      // number of triangular bases
//->   11  
def RC = triang_solve(TS,20);
setring RC;
basering;
//->   //   characteristic : 0 (complex:20 digits, additional 20 digits)
//->   //   1 parameter    : I
//->   //   minpoly        : (I^2+1)
//->   //   number of vars : 5
//->   //        block   1 : ordering lp
//->   //                  : names    x(1) x(2) x(3) x(4) x(5)
//->   //        block   2 : ordering C
size(rlist);   // number of complex solutions
//->   32
rlist[3];      // the third solution
//->   [3]:
//->      [1]:
//->         -0.8
//->      [2]:
//->         (-0.16180339887498948482-I*0.11755705045849462583)
//->      [3]:
//->         (0.06180339887498948482+I*0.19021130325903071442)
//->      [4]:
//->         (0.06180339887498948482-I*0.19021130325903071442)
//->      [5]:
//->         (-0.16180339887498948482+I*0.11755705045849462583)



kill R,R1,RC,j,k;
//================  Example 6.16 (new Session)  ========================
ring R  = 0, (x,y,z), lp;
ideal I = z5+z3-2z2-2, y2+z2+1, x2+2yx-x-y-z2-3;
def F = factorize(I[1],1); F;
//->   F[1]=z3-2
//->   F[2]=z2+1
ring R1 = (0,a), (x,y), lp;
map phi = R,x,y,a;  
minpoly = number(phi(F)[2]); 
ideal Iz = phi(I);              // substitute a for z   
Iz = simplify(Iz,2); Iz;        // remove zero generators
//->   Iz[1]=y2
//->   Iz[2]=x2+2*xy-x-y-2
ideal Izy = reduce(Iz,std(y)); // substitute 0 for y   
Izy = simplify(Izy,2); Izy;
//->   Izy[1]=x2-x-2
def Fzy = factorize(Izy[1],1); Fzy;
//->   Fzy[1]=x-2
//->   Fzy[2]=x+1
ring R2 = (0,b), (x,y), lp;
map phi = R,x,y,b;  
minpoly = number(phi(F)[1]); 
ideal Iz = phi(I);              // substitute a for z   
Iz = simplify(Iz,2); Iz;        // remove zero generators    
//->   Iz[1]=y2+(b2+1)
//->   Iz[2]=x2+2*xy-x-y+(-b2-3)
def Fz = factorize(Iz[1],1);  Fz;
//->   Fz[1]=y2+(b2+1)
if (not(defined(primitive))){ LIB "primitiv.lib"; }
ring S = 0, (b,c), dp;
ideal E = b3-2, c2+(b2+1);
def L = primitive(E); L;
//->   L[1]=c6+3c4+3c2+5
//->   L[2]=1/2c4+c2+1/2
//->   L[3]=c
ring R3 = (0,c), x, lp;
def L = imap(S,L);
map phi = R, x, L[3], L[2];  
minpoly = number(L[1]); 
ideal Izy = simplify(phi(I),2); Izy;
//->   Izy[1]=x2+(2c-1)*x+(c2-c-2)
def Fzy = factorize(Izy[1],1);  Fzy;
//->   Fzy[1]=x+(c+1)
//->   Fzy[2]=x+(c-2)


kill R,R1,R2,R3,S;
//================  Example 6.19 (new Session)  ========================
ring S = 0, (x,y), dp;
poly f, g = xy2-xy-y3+1, x2y2-x2y+xy-1;
poly r = resultant(f,g,x); r;
//->   y8-y7+y6-3y5+y4+y3+y2-y;
factorize(r,2);     // display nonconstant factors only
//->   [1]:
//->      _[1]=y-1
//->      _[2]=y
//->      _[3]=y5+y4+2y3-y-1
//->   [2]:
//->      2,1,1
ideal I1 = eliminate(ideal(f,g),x);
I1;
//->   I1[1]=y6+y4-2y3-y2+1
factorize(I1[1],2);
//->   [1]:
//->      _[1]=y-1
//->      _[2]=y5+y4+2y3-y-1
//->   [2]:
//->      1,1


kill S;
//================  Remark 6.20 (new Session)  ========================
ring S = 0, (x,y), dp;
poly f, g = x2+y2-1, x2+2y2-1;
resultant(f,g,x);
//->   y4
eliminate(ideal(f,g),x);
//->   _[1]=y2 


kill S;
//================  Remark 6.23 (new Session)  ========================
ring R = 0, (x,y,t), dp;
poly f = x-t32;
poly g = y-t48+t56+t60+t62+t63;
int aa = timer;
poly h = resultant(f,g,t);
h; 
//->   x63-595965x62-32x61y+6143174x61+3656768x60y+464x59y2-70859517x60
//->   -65651616x59y-13277840x58y2-4064x57y3+49771514x59+220805184x58y+
//->   [...]
//->   -448x8y27-88x7y28-120x6y28+32x5y29+16x3y30-y32
deg(h);                  // the total degree
//->   63
size(h);                 // number of terms of h
//->   257


kill R,aa;
//================  Example 6.24 (new Session)  ========================
ring R = 0, (x,y,z,t,u(2),u(3)), dp;
poly f(1), f(2), f(3) = x-t, y-t2, z-t3;
poly h = resultant(f(1), u(2)*f(2)+u(3)*f(3), t);
h;
//->   x^3*u(3)+x^2*u(2)-y*u(2)-z*u(3)
ideal CO = coeffs(coeffs(h,u(2)),u(3));
simplify(CO,2);       // remove zeros among generators of CO
//->   _[1]=x^3-z
//->   _[2]=x^2-y
resultant(f(1),f(2),t);
//->   -x^2+y
resultant(f(1),f(3),t);
//->   x^3-z


kill R;
//================  Example 6.32 (new Session)  ========================
ring R = (0,u,v,w), (x,y,z), dp;
poly f1 = x3+y-xy-1;
poly f2 = x2+y2+4x+4y-2;
ideal I = ux+vy+wz, homog(f1,z) , homog(f2,z);
def M0 = mpresmat(I,1);  // the evaluated matrix M_0
nrows(M0);               // the number of rows of M_0
//->   15
poly D0 = det(M0);       // the value of D_0
D0;
//->   12*x6-64*x5y+140*x4y2-352*x3y3+804*x2y4-864*xy5+324*y6-16*x5z+56*x4yz+32*x3y2z+48*x2y3z-336*xy4z+216*y5z-36*x4z2+64*x3yz2-264*x2y2z2+672*xy3z2-564*y4z2+16*x3z3+32*x2yz3-208*xy2z3-160*y3z3+20*x2z4+32*xyz4+236*y2z4-56*yz5+4*z6
int aa = timer;
factorize(D0);
//->   [1]:
//->      _[1]=4
//->      _[2]=3*x3-x2y+9*xy2-27*y3-7*x2z-14*xyz-27*y2z+xz2+11*yz2-z3
//->      _[3]=x-3*y+z
//->      _[4]=x-y-z
//->      _[5]=x-y+z
//->   [2]:
//->      1,1,1,1,1
//->   0

//==============  Example 6.33 (continued session)  ========================
map phi = R,-1,0,z;
poly D0x = phi(D0);
factorize(D0x);           // x-coordinates
//->   [1]:
//->      _[1]=4
//->      _[2]=z-1
//->      _[3]=z3+z2+7*z+3
//->      _[4]=z+1
//->   [2]:
//->      1,2,1,1
map psi = R,0,-1,z;
poly D0y = psi(D0);
factorize(D0y);           // y-coordinates
//->   [1]:
//->      _[1]=4
//->      _[2]=z-1
//->      _[3]=z3+11*z2+27*z-27
//->      _[4]=z+1
//->      _[5]=z+3
//->   [2]:
//->      1,1,1,1,1


kill R,aa;
//================  Example 6.41 (new Session)  ========================
ring R = (0,a(1..3),b(1..3),c(1..3)), (x,y), dp;
poly f0 = a(1)+a(2)*xy+a(3)*y2;
poly f1 = b(1)+b(2)*xy+b(3)*y2;
poly f2 = c(1)*x+c(2)*y+c(3)*xy;
ideal I = f0, f1, f2;  
def M0 = mpresmat(I,0);      // depends on random choices
print(M0);
//->   (b(1)),0,     0,     0,     0,     0,     0,     0,     0,
//->   0,     (a(1)),0,     (b(1)),0,     0,     0,     0,     0,
//->   (b(3)),0,     (a(1)),0,     (b(1)),0,     (c(2)),0,     0,
//->   0,     (a(3)),0,     (b(3)),0,     0,     0,     (c(2)),0,
//->   0,     0,     (a(3)),0,     (b(3)),0,     0,     0,     0,
//->   (b(2)),0,     0,     0,     0,     (b(1)),(c(1)),0,     (a(1)),
//->   0,     (a(2)),0,     (b(2)),0,     0,     (c(3)),(c(1)),0,
//->   0,     0,     (a(2)),0,     (b(2)),(b(3)),0,     (c(3)),(a(3)),
//->   0,     0,     0,     0,     0,     (b(2)),0,     0,     (a(2))
def p = det(M0); 
p;
//->   (-a(1)^3*b(1)*b(2)*b(3)^2*c(3)^2+ [...]

ring S = 0, (a(1..3),b(1..3),c(1..3)), dp;
poly p = imap(R,p);
factorize(p,1);
//->   _[1]=b(1)
//->   _[2]=a(3)^3*b(1)^2*b(2)*c(1)^2-a(2)*a(3)^2*b(1)^2*b(3)*c(1)^2- [...]


kill R,S;
//================  Example 6.45 (new Session)  ========================
ring R = (0,u,v,w), (x,y), dp;
poly f1 = x3+y-xy-1;   
poly f2 = x2+y2+4x+4y-2;
ideal I = u+vx+wy, f1, f2;
def M0 = mpresmat(I,0);  // the evaluated sparse resultant matrix
nrows(M0);
//->   13
def D0 = det(M0);
ring S = 0, (u,v,w), dp;
poly D0 = imap(R,D0);
map phi = S,u,-1,0;
factorize(phi(D0));      // determine x-coordinates of solutions
//->   [1]:
//->      _[1]=2
//->      _[2]=u-1
//->      _[3]=u3+u2+7u+3
//->      _[4]=u+1
//->   [2]:
//->      1,2,1,1
map psi = S,u,0,-1;
factorize(psi(D0));      // determine y-coordinates of solutions
//->   [1]:
//->      _[1]=2
//->      _[2]=u-1
//->      _[3]=u3+11u2+27u-27
//->      _[4]=u+1
//->      _[5]=u+3
//->   [2]:
//->      1,1,1,1,1

map psi_13 = S,u,1,3;
factorize(psi_13(D0));
//->   [1]:
//->      _[1]=2
//->      _[2]=u3-34u2+292u+648
//->      _[3]=u-2
//->      _[4]=u+2
//->      _[5]=u-8
//->   [2]:
//->      1,1,1,1,1


tst_status(1);$

