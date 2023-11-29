LIB "tst.lib"; tst_init();
  LIB "sing.lib";
  ring R=32003,(x,y,z),ds;
  // ---------------------------------------
  // hypersurface case (from series T[p,q,r]):
  int p,q,r = 3,3,4;
  poly f = x^p+y^q+z^r+xyz;
  tjurina(f);
  // Tjurina number = 8
  kbase(Tjurina(f));
  // ---------------------------------------
  // complete intersection case (from series P[k,l]):
  int k,l =3,2;
  ideal j=xy,x^k+y^l+z2;
  dim(std(j));          // Krull dimension
  size(minbase(j));     // minimal number of generators
  tjurina(j);           // Tjurina number
  module T=Tjurina(j);
  kbase(T);             // a sparse output of the k-basis of T_1
  print(kbase(T));      // columns of matrix are a k-basis of T_1
  // ---------------------------------------
  // general case (cone over rational normal curve of degree 4):
  ring r1=0,(x,y,z,u,v),ds;
  matrix m[2][4]=x,y,z,u,y,z,u,v;
  ideal i=minor(m,2);   // 2x2 minors of matrix m
  module M=T_1(i);       // a presentation matrix of T_1
  vdim(M);              // Tjurina number
  hilb(M);              // display of both Hilbert series
  bigintvec v1=hilb(M,1);  // first Hilbert series as intvec
  bigintvec v2=hilb(M,2);  // second Hilbert series as intvec
  v1;
  v2;
  v1[3];                // 3rd coefficient of the 1st Hilbert series
  module N=T_2(i);
tst_status(1);$
