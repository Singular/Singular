  // Two transversal cusps in (k^3,0):
  ring r2 =0,(x,y,z),ds;
  ideal i =z2-1y3+x3y,xz,-1xy2+x4,x3z;
  list resi=mres(i,0);       // computes a minimal resolution
  print(resi[1]);            // the 1-st module is i minimized
  print(resi[2]);            // the 1-st syzygy module of i
  resi[3];                   // the 2-nd syzygy module of i
  ideal j=minor(resi[2],2);
  reduce(j,std(i));          // check whether j is contained in i
  size(reduce(i,std(j)));    // check whether i is contained in j
  // size(<ideal>) counts the non-zero generators
  // ---------------------------------------------
  // The tangent developable of the rational normal curve in P^4:
  ring P = 0,(a,b,c,d,e),dp;
  ideal j= 3c2-4bd+ae, -2bcd+3ad2+3b2e-4ace,
           8b2d2-9acd2-9b2ce+9ac2e+2abde-1a2e2;
  list L=mres(j,0);
  print(L[2]);
  // create an intmat with graded betti numbers
  intmat B=betti(L);
  // this gives a nice output of betti numbers
  print(B,"betti");
  // the user has acess to all betti numbers
  // the 2-nd column of B:
  B[1..4,2];
LIB "tst.lib";tst_status(1);$
