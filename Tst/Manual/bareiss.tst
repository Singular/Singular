LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),(c,dp);
  module mm;
  // ** generation of the module mm **
  int d=7;
  int b=2;
  int db=d-b;
  int i;
  for(i=d;i>0;i--){ mm[i]=3*x*gen(i); }
  for(i=db;i;i--){ mm[i]=mm[i]+7*y*gen(i+b); }
  for(i=d;i>db;i--){ mm[i]=mm[i]+7*y*gen(i-db); }
  for(i=d;i>b;i--){ mm[i]=mm[i]+11*z*gen(i-b); }
  for(i=b;i;i--){ mm[i]=mm[i]+11*z*gen(i+db); }
  // ** the generating matrix of mm **
  print(mm);
  // complete elimination
  list ss=bareiss(mm);
  print(ss[1]);
  ss[2];
  // elimination up to 3 vectors
  ss=bareiss(mm,0,3);
  print(ss[1]);
  ss[2];
  // elimination without the last 3 rows
  ss=bareiss(mm,3,0);
  print(ss[1]);
  ss[2];
tst_status(1);$
