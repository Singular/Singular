LIB "tst.lib";
tst_init();


//======================  Example 5.15 =============================
ring Rxyz = 0, (x,y,z,t), (dp(3),ds(1));
ideal I = x2-t27yz, xz-t13y, x-t14z2;
module M = syz(I);
print(M);
//->   0,      -1,
//->   z2t14-x,zt14,
//->   xz-yt13,x
ring Kxyz = 0, (x,y,z), dp;
ideal I1 = imap(Rxyz,I);
module M1 = imap(Rxyz,M);  // reducing the syzygies
M1 = std(M1);
print(M1);
//->   -z,-1,
//->   x, 0,
//->   0, x
module M2 = syz(I1);   // syzygies on the reductions
print(M2);
//->   0, -1,
//->   -1,0,
//->   z, x
reduce(M2,M1);
//->   _[1]=z*gen(3)-gen(2)
//->   _[2]=0

setring Rxyz;
ideal J = I, -y+tz3;
M = syz(J);
setring Kxyz;
ideal J1 = imap(Rxyz,J);
M1 = imap(Rxyz,M); // reducing the syzygies
M1 = std(M1);
print(M1);
//->   0, -z,-1,0,
//->   -1,x, 0, 0,
//->   z, 0, x, y,
//->   0, 0, 0, x
M2 = syz(J1);    // syzygies on the reductions
print(M2);
//->   0, -1,0,
//->   -1,0, 0,
//->   z, x, y,
//->   0, 0, x
size(reduce(M2,M1));
//->   0


kill Rxyz,Kxyz;
//================= Example 5.17 (new Session) ==========================
ring Kwxyz = 0, (w,x,y,z), dp;
ideal F = x2, xz, wx, w2y;

proc displayHilbPoly(ideal G)
"USAGE:  displayHilbPoly(G), G of type ideal
ASSUME:  G must be a homogeneous Groebner basis for an ideal of the
         active ring in the SINGULAR session; say, G generates the
         homogeneous ideal I of R.
RETURN:  None.
NOTE:    Displays the Hilbert polynomial of R/I.
"
{
  int d = dim(G)-1;       // degree of Hibert polynomial
  bigintvec co = hilb(G,2);  // representation II of Hilbert series
  int s = size(co)-1;     // s = deg(Q_M) +1
  ring Qt = 0, t, dp;     // change active ring to Q[t]
  poly QM = 0;
  for (int i=1; i<=s; i=i+1)
  {
    QM = QM+co[i]*t^(i-1);
  }
  poly QMi = QM;          // the polynomial Q_M(t)
  int ifac = 1;
  list a;
  for (i=1; i<=d+1; i=i+1)
  {
    a = insert(a, subst(QMi,t,1)/ifac, i-1);
    QMi = diff((QMi),t);
    ifac = ifac*i;
  }
  poly PM = (-1)^(d)*a[d+1];
  poly bin = 1;
  for (i=1; i<=d; i=i+1)
  {
    bin = bin*(t+i)/i;    // compute binomial coeff. by recursion
    PM = PM+(-1)^(d-i)*a[d+1-i]*bin;
  }
  print(PM);
}

displayHilbPoly(std(F));  // enter procedure first
//->   3t+1


kill Kwxyz;
//================= Example 5.23 (new Session) ==========================
if (not(defined(isFlat))){ LIB "homolog.lib"; }
ring R = 0, t, dp;
module phi = gen(2)*(t3-t);
isFlat(phi);
//->   0
flatLocus(phi);
//->   _[1]=t3-t

ring S = 0, (x,t(1..2)), (dp(1),dp);
ideal I = x2-t(1), x*t(1)-t(2), x*t(2)-t(1)^2;
std(I);
//->   _[1]=t(1)^3-t(2)^2
//->   _[2]=x*t(2)-t(1)^2
//->   _[3]=x*t(1)-t(2)
//->   _[4]=x^2-t(1)
ring R1 = 0, t(1..2), dp;
module phi = gen(1)*(t(1)^3-t(2)^2),
             gen(2)*(t(1)^3-t(2)^2),
             gen(2)*t(2)-gen(1)*t(1)^2,
             gen(2)*t(1)-gen(1)*t(2);
isFlat(phi);
//->   0
flatLocus(phi);
//->   _[1]=t(1)^3-t(2)^2
qring Q = std(t(1)^3-t(2)^2);
module phi = imap(R1,phi);
isFlat(phi);
//->   0
flatLocus(phi);
//->   _[1]=t(2)
//->   _[2]=t(1)


kill R,S,R1,Q;
//================= Example 5.32 (new Session) ==========================
if (not(defined(KoszulHomology))){ LIB "homolog.lib"; }
ring R = 0, (x,y,z), dp;
ideal f = xz-z, xy-y, x;
module I = 0;            // a presentation matrix of R=Q[x,y,z]
print(KoszulHomology(f,I,0));            // 0th Koszul homology
//->   z,y,x
print(KoszulHomology(f,I,1));            // 1st Koszul homology
//->   0

ideal g = xy, xz, yz;
print(KoszulHomology(g,I,0));            // 0th Koszul homology
//->   yz,xz,xy
print(KoszulHomology(g,I,1));            // 1st Koszul homology
//->   -z,0,x,
//->   z, y,0
print(KoszulHomology(g,I,2));            // 2nd Koszul homology
//->   0


kill R;
//================= Example 5.34 (new Session) ==========================
//=============== continuation of Example 5.34 ==========================
ring S = 32003, x(0..4), dp;
module MI=maxideal(1);
attrib(MI,"isHomog",intvec(-1));
resolution kos = nres(MI,0);
print(betti(kos),"betti");
//->              0     1     2     3     4     5
//->   ------------------------------------------
//->      -1:     1     5    10    10     5     1
//->   ------------------------------------------
//->   total:     1     5    10    10     5     1

matrix alpha0 = random(32002,10,3);
module pres = module(alpha0)+kos[3];

attrib(pres,"isHomog",intvec(1,1,1,1,1,1,1,1,1,1));
resolution fcokernel = mres(pres,0);
print(betti(fcokernel),"betti");
//->              0     1     2     3
//->   ------------------------------
//->       1:     7    10     5     1
//->   ------------------------------
//->   total:     7    10     5     1

module dir = transpose(pres);
intvec w = -1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2;
attrib(dir,"isHomog",w);
resolution fdir = mres(dir,2);
print(betti(fdir),"betti");
//->              0     1     2
//->   ------------------------
//->      -2:    10     7     -
//->      -1:     -     -     -
//->       0:     -     -     1
//->   ------------------------
//->   total:    10     7     1

if (not(defined(flatten))){ LIB "matrix.lib"; }
ideal I = groebner(flatten(fdir[2]));
resolution FI = mres(I,0);
print(betti(FI),"betti");
//->              0     1     2     3     4
//->   ------------------------------------
//->       0:     1     -     -     -     -
//->       1:     -     -     -     -     -
//->       2:     -     7    10     5     1
//->   ------------------------------------
//->   total:     1     7    10     5     1

int codimI = nvars(S)-dim(I);
codimI;
//->   2
degree(I);
//->   4
nvars(S)-dim(groebner(minor(jacob(I),codimI) + I));
//->   5

if (not(defined(depth))){ LIB "homolog.lib"; }
depth(I); // I is a presentation matrix of S/I
//->   1
dim(std(I));
//->   3


//================= Example 5.46 (new Session) ==========================
if (not(defined(isCM))){ LIB "homolog.lib"; }
ring R = 0, (x,y,z,w), dp;
ideal I = xz-y2, wz-xy, wy-x2;
ring R_loc = 0, (x,y,z,w), ds;
ideal I = imap(R,I);
isCM(I);
//->   1


kill R,S,w,codimI;
//================= Example 5.49 (new Session) ==========================
ring R = 0, (x,y,z,w), dp;
ideal I = xz-y2, wz-xy, wy-x2;
I=std(I);
lead(I);               // the leading ideal
//->   _[1]=y2
//->   _[2]=xy
//->   _[3]=x2

tst_status(1);$

