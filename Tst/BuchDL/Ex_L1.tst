LIB "tst.lib";
tst_init();


//======================  Example 1.19 =============================
ring R = 0, (w,x,y,z), dp;
ideal I = xz-y2, wz-xy, wy-x2;
I;
//->    I[1]=-y2+xz
//->    I[2]=-xy+wz
//->    I[3]=-x2+wy

resolution fI = mres(I,0);
fI;
//->     1      3      2
//->    R <--  R <--  R
//->    0      1      2

print(fI[1]);   // the first syzygy matrix of R/I
//->    y2-xz,
//->    xy-wz,
//->    x2-wy

print(fI[2]);   // the second syzygy matrix of R/I
//->    x, w,
//->    -y,-x,
//->    z, y

print(betti(fI),"betti");   // the Betti diagram
                            // (see the remark below)
//->               0     1     2
//->    ------------------------
//->        0:     1     -     -
//->        1:     -     3     2
//->    ------------------------
//->    total:     1     3     2


//==================  continued in Example 1.26 ====================
ideal GI = groebner(I);
hilb(GI);
//->    //         1 t^0
//->    //        -3 t^2
//->    //         2 t^3
//->
//->    //         1 t^0
//->    //         2 t^1
//->    // dimension (proj.)  = 1
//->    // degree      = 3

bigintvec co1 = hilb(GI,1);
co1;
//->    1,0,-3,2,0
bigintvec co2 = hilb(GI,2);
co2;
//->    1,2,0

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


displayHilbPoly(GI);
//->    3t+1


kill R,co1,co2;
//======================  Example 1.39 (New Session) ======================
ring R = 0, (x,y), dp;
ideal I = x2y-y3, x3;
poly f = x3y+x3;
reduce(f,I,1);
//->    // ** I is no standard basis
//->    xy3+x3

reduce(f,I);
//->    // ** I is no standard basis
//->    xy3

ideal GI = groebner(I);
GI;
//->    GI[1]=x2y-y3
//->    GI[2]=x3
//->    GI[3]=xy3
//->    GI[4]=y5
reduce(f,GI);
//->    0

tst_status(1);$

