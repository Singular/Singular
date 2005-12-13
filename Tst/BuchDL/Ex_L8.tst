LIB "tst.lib";
tst_init();


//======================  Example 8.6 =============================
LIB "finvar.lib";        
ring R = (0,a), (x(0..4)), dp;
minpoly = a4+a3+a2+a+1;  // need fifth roots of unity
matrix Si[5][5] = 0,0,0,0,1,
                  1,0,0,0,0,
                  0,1,0,0,0,
                  0,0,1,0,0,
                  0,0,0,1,0;
matrix Ta[5][5];
Ta[1,1] = 1;  Ta[2,2] = a;  Ta[3,3] = a2;  Ta[4,4] = a3;  
Ta[5,5] = a4;
int aa = timer;          // time in seconds
matrix P,S,IS = invariant_ring(Si,Ta,intvec(0,0,1));

//->   Generating the entire matrix group. Whenever a new group element is
//->   found, the corresponding ring homomorphism of the Reynolds operator
//->   and the corresponding term of the Molien series is generated.

//->   Group element 3 has been found.
//->                        [...]
//->   Group element 125 has been found.

//->   Now we are done calculating Molien series and Reynolds operator.

//->   We can start looking for primary invariants...

//->   Computing primary invariants in degree 5:
//->   We find: x(0)*x(1)*x(2)*x(3)*x(4)
//->   We find: x(0)^3*x(2)*x(3)+x(0)*x(1)*x(3)^3+x(0)*x(2)^3*x(4)+[...]
//->   We find: x(0)*x(1)^3*x(2)+x(1)*x(2)^3*x(3)+x(0)^3*x(1)*x(4)+[...]
//->   Computing primary invariants in degree 10:
//->   We find: x(0)^10+x(1)^10+x(2)^10+x(3)^10+x(4)^10
//->   We find: -x(0)^5*x(1)^5+x(0)^5*x(2)^5-[...]

//->   We found all primary invariants.

//->   Polynomial telling us where to look for secondary invariants:
//->    x(0)^30+3*x(0)^25+24*x(0)^20+44*x(0)^15+24*x(0)^10+3*x(0)^5+1

//->   In degree 0 we have: 1

//->   Searching in degree 5, we need to find 3 invariant(s)...
//->   We find: x(0)^2*x(1)^2*x(3)+x(0)*x(2)^2*x(3)^2+[...]
//->   We find: x(0)^2*x(1)*x(2)^2+x(1)^2*x(2)*x(3)^2+[...]
//->   We find: x(0)^5+x(1)^5+x(2)^5+x(3)^5+x(4)^5 .

//->   Searching in degree 10, we need to find 24 invariant(s)...
//->                        [...]   
//->   Searching in degree 15, we need to find 44 invariant(s)...
//->                        [...]   
//->   Searching in degree 20, we need to find 24 invariant(s)...
//->                        [...]   
//->   Searching in degree 25, we need to find 3 invariant(s)...
//->                        [...]
//->   Searching in degree 30, we need to find 1 invariant(s)...
//->   We find: x(0)^15*x(1)^10*x(3)^5+x(0)^10*x(1)^15*x(3)^5+[...]

//->   We're done!

ideal HMQ = invariant_basis(5,Si,Ta);
print(HMQ);
//->   x(0)*x(1)*x(2)*x(3)*x(4),
//->   x(0)^2*x(1)*x(2)^2+x(1)^2*x(2)*x(3)^2+x(0)^2*x(3)^2*x(4)+[...]
//->   x(0)^2*x(1)^2*x(3)+x(0)*x(2)^2*x(3)^2+x(1)^2*x(2)^2*x(4)+[...]
//->   x(0)^3*x(2)*x(3)+x(0)*x(1)*x(3)^3+x(0)*x(2)^3*x(4)+[...]
//->   x(0)*x(1)^3*x(2)+x(1)*x(2)^3*x(3)+x(0)^3*x(1)*x(4)+[...]
//->   x(0)^5+x(1)^5+x(2)^5+x(3)^5+x(4)^5


kill R,aa;
//==================  Remark 8.7(new Session) =========================
if (not(defined(invariant_ring))){ LIB "finvar.lib"; }
ring R = 101, (x(0..4)), dp;
matrix Si[5][5] = 0,0,0,0,1,
                  1,0,0,0,0,
                  0,1,0,0,0,
                  0,0,1,0,0,
                  0,0,0,1,0;
number a = 36;              // primitive fifth root of unity
matrix Ta[5][5];
Ta[1,1] = 1;  Ta[2,2] = a;  Ta[3,3] = a^2;  Ta[4,4] = a^3;
Ta[5,5] = a^4;
int aa = timer;             // time in seconds
matrix P,S,IS = invariant_ring(Si,Ta,intvec(0,0,0));
size(S);
//->   100
print(S[100]);
//->   [x(0)^15*x(1)^10*x(3)^5+x(0)^10*x(1)^15*x(3)^5+[...]
ideal HMQ = invariant_basis(5,Si,Ta);

//==================  Example 8.8 (continued Session) ===================
ring P4 = 101, (x(0..4)), dp;
ideal HMQ = fetch(R,HMQ);
ideal CI = HMQ[1], HMQ[2];
list DEG = primdecGTZ(CI);
size(DEG);
//->   10
ideal I6 = DEG[6][1]; I6;
//->   I6[1]=x(1)^2*x(3)+x(2)*x(4)^2
//->   I6[2]=x(0)
degree(std(I6));
//->   // dimension (proj.)  = 2
//->   // degree (proj.)   = 3
ideal I10 = DEG[10][1]; I10;
//->   I10[1]=x(2)^2
//->   I10[2]=x(2)*x(3)^2+x(0)*x(4)^2
//->   I10[3]=x(0)*x(2)
//->   I10[4]=x(0)^2
degree(std(I10));
//->   // dimension (proj.)  = 2
//->   // degree (proj.)   = 2
ideal IX = intersect(DEG[3][1],DEG[5][1],DEG[8][1],DEG[9][1], 
                     DEG[10][1]);
degree(std(IX));
//->   // dimension (proj.)  = 2
//->   // degree (proj.)   = 10
resolution FIX = mres(IX,0);
print(betti(FIX),"betti");
//->              0     1     2     3     4
//->   ------------------------------------
//->       0:     1     -     -     -     -
//->       1:     -     -     -     -     -
//->       2:     -     -     -     -     -
//->       3:     -     -     -     -     -
//->       4:     -     3     -     -     -
//->       5:     -    15    35    20     -
//->       6:     -     -     -     -     2
//->   ------------------------------------
//->   total:     1    18    35    20     2

module N = transpose(FIX[3]);
homog(N);
//->   1
intvec deg_N = attrib(N,"isHomog");
attrib(N,"isHomog",deg_N-3);        // set degrees
resolution FN = mres(N,0);
print(betti(FN),"betti");
//->              0     1     2     3     4     5
//->   ------------------------------------------
//->      -3:    20    35    15     -     -     -
//->      -2:     -     -     4     -     -     -
//->      -1:     -     -     -     -     -     -
//->       0:     -     -     5    15    10     2
//->   ------------------------------------------
//->   total:    20    35    24    15    10     2
matrix NN = FN[2];
matrix PRESMHM[35][19] = NN[1..35,1..19];
PRESMHM = transpose(PRESMHM);
resolution FMHM = mres(PRESMHM,0);
print(betti(FMHM),"betti");
//->              0     1     2     3
//->   ------------------------------
//->       0:     4     -     -     -
//->       1:    15    35    20     -
//->       2:     -     -     -     2
//->   ------------------------------
//->   total:    19    35    20     2

matrix zero[1][15];
matrix ran = random(100,1,4);
matrix psi = transpose(concat(zero,ran));
matrix pres = PRESMHM + module(psi);
module dir = transpose(pres);
resolution fdir = mres(dir,2);
print(betti(fdir),"betti");
//->              0     1     2
//->   ------------------------
//->       0:    35    15     -
//->       1:     -     3     -
//->       2:     -     -     -
//->       3:     -     -     -
//->       4:     -     -     -
//->       5:     -     -     1
//->   ------------------------
//->   total:    35    18     1
ideal IA = groebner(flatten(fdir[2]));
int codimIA = nvars(P4) - dim(IA);
ideal sIA = minor(jacob(IA),codimIA)+IA;
nvars(P4) - dim(groebner(sIA));
//->   5

matrix dummy[1][3] = IA[1..3];     // the 3 quintics in IA
ideal CI2 = dummy*random(100,3,2);
ideal IA' = sat(CI2,IA)[1];
resolution FIA' = mres(IA',0);
print(betti(FIA'),"betti");
//->              0     1     2     3     4
//->   ------------------------------------
//->       0:     1     -     -     -     -
//->       1:     -     -     -     -     -
//->       2:     -     -     -     -     -
//->       3:     -     -     -     -     -
//->       4:     -     3     -     -     -
//->       5:     -     -     -     -     -
//->       6:     -     5    15    10     2
//->   ------------------------------------
//->   total:     1     8    15    10     2
int codimIA' = nvars(P4) - dim(IA');
ideal sIA' = minor(jacob(IA'),codimIA')+IA';
nvars(P4) - dim(groebner(sIA'));
//->   5

ideal QA = IA[1..3];
ideal HMlines = sat(QA,IA)[1];     // result is a Groebner basis
degree(HMlines);
//->   // dimension (proj.)  = 1
//->   // degree (proj.)   = 25


kill R,aa,P4,codimIA,codimIA',deg_N;
//================== Example 8.9 (new Session) =========================
ring R = 2, (x(1..4)), dp;
matrix A[4][4];
A[1,4] = 1;  A[2,1] = 1;  A[3,2] = 1;  A[4,3] = 1;
print(A);
//->   0,0,0,1,
//->   1,0,0,0,
//->   0,1,0,0,
//->   0,0,1,0 
if (not(defined(invariant_ring))){ LIB "finvar.lib"; }
matrix P,S = invariant_ring(A);
P;
//->   P[1,1]=x(1)+x(2)+x(3)+x(4)
//->   P[1,2]=x(1)*x(3)+x(2)*x(4)
//->   P[1,3]=x(1)*x(2)+x(2)*x(3)+x(1)*x(4)+x(3)*x(4)
//->   P[1,4]=x(1)*x(2)*x(3)*x(4)
size(S);
//->   5


kill R;
//================== Example 8.10 (new Session) =========================
if (not(defined(invariant_ring))){ LIB "finvar.lib"; }
ring R = (0,a), (x(0..3)), dp;
minpoly = a4+a3+a2+a+1;
matrix A[4][4];
A[1,1] = a;  A[2,2] = a2;  A[3,3] = a3;  A[4,4] = a4;
matrix P,S,IS = invariant_ring(A,intvec(0,0,0));
size(P);
//->   4
size(S);
//->   12

proc min_generating_set (matrix P,S)
"USAGE:  min_generating_set(P,S);   P,S matrix
ASSUME: The entries of P,S are homogeneous and ordered by ascending 
        degrees. The first entry of S equals 1. (As satisfied by 
        the first two output matrices of invariant_ring(G).)
RETURN: ideal
NOTE:   The given generators for the output ideal form a minimal 
        generating set for the ring generated by the entries of 
        P,S. The generators are homogeneous and ordered by 
        descending degrees.
"
{
  if (defined(flatten)==0) { LIB "matrix.lib"; }
  ideal I1,I2 = flatten(P),flatten(S); 
  int i1,i2 = size(I1),size(I2);
  // We order the generators by descending degrees
  // (the first generator 1 of I2 is omitted):
  int i,j,s = i1,i2,i1+i2-1;
  ideal I;
  for (int k=1; k<=s; k++)
  { 
    if (i==0) { I[k]=I2[j]; j--; } 
    else
    { 
      if (j==0) { I[k]=I1[i]; i--; } 
      else 
      {
        if (deg(I1[i])>deg(I2[j])) { I[k]=I1[i]; i--; }
        else { I[k]=I2[j]; j--; }
      }
    }
  } 
  intvec deg_I = deg(I[1..s]);
  int n = nvars(basering);
  def BR = basering;

  // Create a new ring with elimination order:
  //---------------------------------------------------------------
  // ****    this part uses the command ringlist which is      ****
  // ****     only available in SINGULAR-3-0-0 or newer        ****
  //---------------------------------------------------------------
  list rData = ringlist(BR);
  intvec wDp;
  for (k=1; k<=n; k++) { 
    rData[2][k] ="x("+string(k)+ ")"; 
    wDp[k]=1;
  }
  for (k=1; k<=s; k++) { rData[2][n+k] ="y("+string(k)+ ")"; } 
  rData[3][1] = list("dp",wDp);
  rData[3][2] = list("wp",deg_I);
  def R_aux = ring(rData);
  setring R_aux;
  //---------------------------------------------------------------

  ideal J;
  map phi = BR, x(1..n);
  ideal I = phi(I);
  for (k=1; k<=s; k++) { J[k] = y(k)-I[k]; } 
  option(redSB);
  J = std(J);

  // Remove all generators that are depending on some x(i) from J: 
  int s_J = size(J);
  for (k=1; k<=s_J; k++) { if (J[k]>=x(n)) {J[k]=0;} }

  // The monomial order on K[y] is chosen such that linear leading 
  // terms in J are in 1-1 correspondence to superfluous generators
  // in I :
  ideal J_1jet = std(jet(lead(J),1));
  intvec to_remove; 
  i=1;
  for (k=1; k<=s; k++)
  { 
    if (reduce(y(k),J_1jet)==0){ to_remove[i]=k; i++; }
  }
  setring BR;
  if (to_remove == 0) { return(ideal(I)); }
  for (i=1; i<=size(to_remove); i++)
  { 
    I[to_remove[i]] = 0;
  } 
  I = simplify(I,2);
  return(I);
}

ideal FSI = min_generating_set(P,S);
size(FSI);
//->   14
ring Rnew = 0, (x(0..3)), dp;  // coefficient field is now Q
ideal FSI = fetch(R,FSI);
ideal ZERO;
ring R1 = 0, (y(0..13)), wp(5,5,5,5,4,4,4,4,3,3,3,3,2,2);
ideal REL = preimage(Rnew,FSI,ZERO);
homog(REL);                    // check that REL is homogeneous
//->   1
size(REL);
//->   54
setring Rnew;
FSI[4];
//->   x(0)^5+x(1)^5+x(2)^5+x(3)^5
ideal F = FSI[4];
setring R1;
ideal GODEAUX = preimage(Rnew,FSI,F);
size(GODEAUX);
//->   55
GODEAUX[1];
//->   y(3)
dim(std(GODEAUX));
//->   3

tst_status(1);$

