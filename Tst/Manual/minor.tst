LIB "tst.lib"; tst_init();
  ring r=0,(a,b,c,d,e,f,g,h,s,t,u,v),ds;
  matrix m[3][4]=a,b,c,d,e,f,g,h,s,t,u,v;
  print(m);
  // let's compute all non-zero minors;
  // here we do not guarantee any ordering:
  minor(m,2);
  ideal i=a,c; i=std(i);
  // here come the first 4 non-zero minors mod I;
  // this time, a fixed ordering is guaranteed:
  minor(m,2,i,4);
  // and here the first 4 minors mod I (possibly zero)
  // using Laplace's algorithm,
  // again, the fixed ordering is guaranteed:
  minor(m,2,i,-4,"Laplace");
tst_status(1);$
