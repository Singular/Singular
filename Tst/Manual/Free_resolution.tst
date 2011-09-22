LIB "tst.lib"; tst_init();
  // Two transversal cusps in (k^3,0):
  ring r2 =0,(x,y,z),ds;
  ideal i =z2-1y3+x3y,xz,-1xy2+x4,x3z;
  resolution rs=mres(i,0);   // computes a minimal resolution
  rs;                        // the standard representation of complexes
    list resi=rs;            // convertion to a list
  print(resi[1]);            // the 1st module is i minimized
  print(resi[2]);            // the 1st syzygy module of i
  resi[3];                   // the 2nd syzygy module of i
  ideal j=minor(resi[2],2);
  reduce(j,std(i));          // check whether j is contained in i
  size(reduce(i,std(j)));    // check whether i is contained in j
  // size(<ideal>) counts the non-zero generators
  // ---------------------------------------------
  // The tangent developable of the rational normal curve in P^4:
  ring P = 0,(a,b,c,d,e),dp;
  ideal j= 3c2-4bd+ae, -2bcd+3ad2+3b2e-4ace,
           8b2d2-9acd2-9b2ce+9ac2e+2abde-1a2e2;
  resolution rs=mres(j,0);
  rs;
  list L=rs;
  print(L[2]);
  // create an intmat with graded Betti numbers
  intmat B=betti(rs);
  // this gives a nice output of Betti numbers
  print(B,"betti");
  // the user has access to all Betti numbers
  // the 2-nd column of B:
  B[1..4,2];
  ring cyc5=32003,(a,b,c,d,e,h),dp;
  ideal i=
  a+b+c+d+e,
  ab+bc+cd+de+ea,
  abc+bcd+cde+dea+eab,
  abcd+bcde+cdea+deab+eabc,
  h5-abcde;
  resolution rs=lres(i,0);   //computes the resolution according LaScala
  rs;                        //the shape of the minimal resolution
  print(betti(rs),"betti");  //shows the Betti-numbers of cyclic 5
  dim(rs);                   //the homological dimension
  size(list(rs));            //gets the full (non-reduced) resolution
  minres(rs);                //minimizes the resolution
  size(list(rs));            //gets the minimized resolution
tst_status(1);$
