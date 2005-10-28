LIB "tst.lib";
tst_init();


//======================  Example 9.5 =============================
ring R = 0, x, dp;    // global order: x>1
ideal I = x2+x;
std(I);
//->   _[1]=x2+x
ring S = 0, x, ds;    // local order: 1>x
ideal I = x2+x;
std(I);
//->   _[1]=x


kill R,S;
//=================== Lemma 9.11 (new Session) =====================
ring R = 0, (v,w), M(-2,1,0,-1);
poly f = 1-vw;
reduce(1,std(ideal(f))); // output shows: f is a unit in R
//->   0
ring S = 0, (v,w), M(-2,3,0,-1);
map phi = R,v,w;         // implementing a nonexisting ring map
reduce(1,std(phi(f)));   // output shows: f is not a unit in S
//->   1


kill R,S;
//=================== Example 9.27 (new Session) =====================
ring R = 0, (x,y), ds;   // local monomial order
poly f = y13-y15+x2y9+x2y10-2x2y11-x2y12+x2y13-3x3y11+3x3y13+x4y6
-x4y7-2x4y8+x4y9+x4y10-3x5y7-3x5y8+5x5y9-x5y10-2x5y11+4x5y12-x6y4
+x6y6+3x6y9-3x6y11-3x7y4+2x7y5+x7y6-6x7y7+6x7y8+3x7y9-4x7y10+x7y11
+3x8y5+3x8y6-4x8y7+x8y8+x8y9-4x8y10+2x9y2-4x9y3+2x9y4+7x9y5-5x9y6
-3x9y7+x9y8+3x10y2-x10y3+5x10y5-3x10y6-3x10y7-x10y9+4x11y-x11y2
-4x11y3+x11y4-x12+4x12y+x12y2-3x12y3+x12y4-x12y5-x12y6+x14-x14y2;
factorize(f);  
//->   [1]:
//->      _[1]=-1
//->      _[2]=-y4+2x3y2-x6+4x5y+x7
//->      _[3]=x2-y2+x3
//->      _[4]=x2+y3
//->      _[5]=x2+y4
//->      _[6]=-1+y
//->      _[7]=1+y
//->   [2]:
//->      1,1,1,1,1,1,1

//============= Example 9.48 (continuation of session) ===============
if (not(defined(hnexpansion))){ LIB "hnoether.lib"; }
list L = hnexpansion(f);
def HNring = L[1];
show(HNring);
//->   // ring: (0,a),(x,y),(ls(2),C);
//->   // minpoly = (a2+1)
//->   // objects belonging to this ring:
//->   // f                    [0]  poly
//->   // hne                  [0]  list, size: 6
setring HNring;
displayHNE(hne);
//->   // Hamburger-Noether development of branch nr.1:
//->     y = x+1/2*x^2 + ..... (terms of degree >=3)

//->   // Hamburger-Noether development of branch nr.2:
//->     y = -x-1/2*x^2 + ..... (terms of degree >=3)

//->   // Hamburger-Noether development of branch nr.3:
//->     y = z(1)*x
//->     x = z(1)^2+z(1)^2*z(2)
//->     z(1) = 1/4*z(2)^2-1/2*z(2)^3 + ..... (terms of degree >=4)

//->   // Hamburger-Noether development of branch nr.4:
//->     x = z(1)*y
//->     y = -z(1)^2
 
//->   // Hamburger-Noether development of branch nr.5:
//->     x = (a)*y^2

//->   // Hamburger-Noether development of branch nr.6:
//->     x = (-a)*y^2
 
ring R_param = (0,a), (x,y,t), ls;
minpoly = a2+1;
list hne = imap(HNring,hne);
list P = param(hne[3],1);
//->   // ** Warning: result is exact up to order 5 in x and 7 in y !
P[1];
//->   _[1]=1/16*t4-3/16*t5+1/4*t7
//->   _[2]=1/64*t6-5/64*t7+3/32*t8+1/16*t9-1/8*t10
hne[3] = extdevelop(hne[3],5);
P = param(hne[3],1);
//->   // ** Warning: result is exact up to order 7 in x and 9 in y !
P[1];
//->   _[1]=1/16*t4-3/16*t5+47/128*t6-19/32*t7+ [...]
//->   _[2]=1/64*t6-5/64*t7+237/1024*t8-271/512*t9+ [...]

displayInvariants(hne);
//->   [...]
//->   --- invariants of branch number 3 : ---
//->   characteristic exponents  : 4,6,7
//->   generators of semigroup   : 4,6,13
//->   Puiseux pairs             : (3,2)(7,2)
//->   degree of the conductor   : 16
//->   delta invariant           : 8
//->   sequence of multiplicities: 4,2,2,1,1
//->   [...] 
//->    -------------- intersection multiplicities : -------------- 

//->   branch |    6     5     4     3     2    
//->   -------+-----------------------------
//->       1  |    1,    1,    2,    4,    1
//->       2  |    1,    1,    2,    4
//->       3  |    4,    4,    8
//->       4  |    3,    3
//->       5  |    2
//->   [...]


kill R,L,HNring,R_param;
//=================== Example 9.31 (new Session) =====================
ring R = 0, (x,y,z), dp;
ideal I = x*(x2-y2z), (y-z)*(x2-y2z);
ideal G = std(I); G;       // compute Groebner basis
//->   G[1]=y3z-y2z2-x2y+x2z
//->   G[2]=xy2z-x3
dim(G);                    // global dimension
//->   2
ring R1 = 0, (x,y,z), ds;  // implements local ring at (0,0,0) 
ideal I = imap(R,I);
ideal G = std(I); G;       // compute standard basis
//->   G[1]=x3-xy2z
//->   G[2]=x2y-x2z-y3z+y2z2
dim(G);                    // local dimension at (0,0,0)
//->   2
map phi = R,x,y-1,z-1;     // translate (0,1,1) to the origin
I = phi(I);
G = std(I); G;
//->   G[1]=x
//->   G[2]=y-z-2y2+yz+z2+x2y+y3-x2z+y2z-2yz2-y3z+y2z2
dim(G);                    // local dimension at (0,1,1)
//->   1
map psi = R,x,y,z+1;       // translate (0,0,-1) to the origin
I = psi(I);
G = std(I);
G;
//->   G[1]=x2-y2-x2y+y3+x2z-2y2z+y3z-y2z2
dim(G);                    // local dimension at (0,0,-1)
//->   2
setring R;
if (not(defined(minAssGTZ))){ LIB "primdec.lib"; }
list L = minAssGTZ(G); L;
//->   [1]:
//->      _[1]=y-z
//->      _[2]=x
//->   [2]:
//->      _[1]=-y2z+x2
ideal SLoc = L[2][1], jacob(L[2][1]);
radical(SLoc);
//->   _[1]=y
//->   _[2]=x


kill R,R1;
//=================== Example 9.36 (new Session) =====================
ring S = 0, (x,y), ds;     // the local ring 
poly f = y2-2x28y-4x21y17+4x14y33-8x7y49+x56+20y65+4x49y16; 
ideal I = f, jacob(f);
vdim(std(I));              // Tjurina number at the origin
//->   2260 
ring R = 0, (x,y), dp;      // the affine ring Q[x,y]
ideal I = fetch(S,I);
vdim(std(I));
//->   2260 
ring SH = 0, (x,y,z), dp; 
poly f = fetch(S,f); 
poly fhom = homog(f,z); 
ring R1 = 0, (y,z), dp; 
map phi = SH,1,y,z; 
poly g = phi(fhom);         // fhom in the affine chart (x=1) 
g;
//->   20y65+y2z63-8y49z9+4y33z18-4y17z27-2yz36+4y16+z9
ideal J = g, jacob(g); 
vdim(std(J));
//->   120 
ring R2 = 0, (y,z), ds;     // the local ring at (1:0:0) 
ideal J = fetch(R1,J); 
vdim(std(J));
//->   120 


kill S,R,SH,R1,R2;
//=================== Example 9.39 (new Session) =====================
ring R_loc = 0, (x,y), ds;    // local monomial order
poly f, g = 2x2-y3, 2x2-y5;
ideal I = f, g;   
vdim(std(I));  // intersection multiplicity at origin
//->   6
LIB "surf.lib";
//   plot(f,"scale_x=0.15; scale_y=0.15;");
//   plot(g,"scale_x=0.15; scale_y=0.15;");
//   plot(f*g,"scale_x=0.15; scale_y=0.15;");
ring R_aff = 0, (x,y), dp;    // global monomial order
ideal I = imap(R_loc,I);
vdim(std(I));
//->   10
if (not(defined(primdecGTZ))){ LIB "primdec.lib"; } 
primdecGTZ(I);
//->   [1]:
//->      [1]:
//->         _[1]=y3
//->         _[2]=x2
//->      [2]:
//->         _[1]=y
//->         _[2]=x
//->   [2]:
//->      [1]:
//->         _[1]=y-1
//->         _[2]=2x2-1
//->      [2]:
//->         _[1]=y-1
//->         _[2]=2x2-1
//->   [3]:
//->      [1]:
//->         _[1]=y+1
//->         _[2]=2x2+1
//->      [2]:
//->         _[1]=y+1
//->         _[2]=2x2+1
ring Rt_loc = (0,t), (x,y), ds;  // choose coefficient field Q(t)
poly F = imap(R_loc,f)-(t/2)*y2;
poly G = imap(R_loc,g)+(5t2/4)*y3-(5t4/16)*y-(t5/16);
ideal I = F, G;
vdim(std(I));   // intersection number at (0,0) for general fiber
//->   0
ring Rt_aff = (0,t), (x,y), dp;
ideal I = imap(Rt_loc,I);    
vdim(std(I));   // sum of intersection numbers for general fiber
//->   10
primdecGTZ(I);
//->   [1]:
//->      [1]:
//->         _[1]=16*y5+(-20t2-16)*y3+(-8t)*y2+(5t4)*y+(t5)
//->         _[2]=-2*y3+4*x2+(-t)*y2
//->      [2]:
//->         _[1]=16*y5+(-20t2-16)*y3+(-8t)*y2+(5t4)*y+(t5)
//->         _[2]=-2*y3+4*x2+(-t)*y2
setring Rt_loc;
poly f_def = subst(F,t,1/10);
poly g_def = subst(G,t,1/10);
setring R_loc;
poly f_def = imap(Rt_loc,f_def);
poly g_def = imap(Rt_loc,g_def);
//   plot(f_def,"scale_x=0.001; scale_y=0.015;");
//   plot(g_def,"scale_x=0.001; scale_y=0.015;");
map phi = R_loc, 1/1000*x, 1/10*y;               // zooming map
//   plot(10000000*phi(f_def)*phi(g_def),"scale_x=0.5;scale_y=0.075;");
if (not(defined(tjurina))){ LIB "sing.lib"; }
tjurina(f);
//->   2
milnor(f);
//->   2
tjurina(g);
//->   4
milnor(g);
//->   4
setring Rt_loc;
tjurina(F);     // Tjurina number at origin for general fiber
//->   1
setring Rt_aff;
poly F = imap(Rt_loc,F);
tjurina(F);     // sum of Tjurina numbers for general fiber
//->   1
poly G = imap(Rt_loc,G);
tjurina(G);
//->   2
setring Rt_loc;
tjurina(G);
//->   0
setring Rt_aff;
milnor(F);        // sum of Milnor numbers for general fiber
//->   2
milnor(G);
//->   4
setring R_loc;
poly h0 = diff(g,x); 
poly h0_prime = diff(g,x)+diff(g,y);
poly hc = diff(g_def,x); 
poly hc_prime = diff(g_def,x)+diff(g_def,y);
//   plot(h0*h0_prime,"scale_x=0.15; scale_y=0.15;");
//   plot(hc*hc_prime,"scale_x=0.000005; scale_y=0.015;"); 


kill R_loc,Rt_loc,R_aff,Rt_aff;
//=================== Remark 9.41 (new Session) =====================
ring R = 0, (x,y,z), dp;
ideal I0 = x-z, y-z^2;       // vanishing at the origin
ideal I1 = x-z+1, y-z^2+1;   // not vanishing at the origin
ideal I = intersect(I0,I1); 
I = std(I); I;
//->   _[1]=z2+x-y-z
//->   _[2]=x2-2xz+y
ring Rloc = 0, (x,y,z), ds; 
ideal Iloc = fetch(R,I);
Iloc = std(Iloc); Iloc;
//->   _[1]=x-y-z+z2
//->   _[2]=y+x2-2xz
ideal J = eliminate(Iloc,z);
J;
//->   J[1]=2xy-y2-2x3+2x2y-x4
factorize(J[1],1);
//->   _[1]=-2x+y-x2
//->   _[2]=-y+x2
ideal I0loc = fetch(R,I0);
eliminate(I0loc,z);
//->   _[1]=y-x2


kill R,Rloc;
//=================== Example 9.42 (new Session) =====================
ring R = 0, (t,x,y,z), ds;
ideal I = x-t+t3, y-t+t2, z-t;
ideal C = eliminate(I,t); C;
//->   C[1]=x-z+z3
//->   C[2]=y-z+z2
eliminate(C,z);
//->   _[1]=x2-3xy+2y2+y3
//   plot(x2-3xy+2y2+y3,"scale_x=0.05;scale_y=0.05;");


kill R;
//=================== Remark 9.49 (new Session) =====================
ring R = 0, (x,y,t), ds;
ideal J = x-t4, y-t6-t7;
eliminate(J,t);
//->   _[1]=y4-2x3y2+x6-4x5y-x7

tst_status(1);$

