  intmat m[2][3]=1,0,2,4,5,1;
  m+1;             // adds unit intmat to m
  m[2,1];          // entry at 2,1
  intmat m1[3][2]=1,0,2,4,0,1;
  m1*10;           // multiply each entry with 10
  m*m1;
  m+transpose(m1);
  intvec v1=1,2,4;
  intvec v2=5,7,8;
  m=v1,v2;         // overwrite M with v1 and v2
  m;
  trace(m);
$
