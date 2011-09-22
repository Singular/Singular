LIB "tst.lib"; tst_init();
  intmat Idm[2][2];
  Idm +1;          // add the unit intmat
  intmat m1[3][2] = _,1,-2;  // take entries from the last result
  m1;
  intmat m2[2][3]=1,0,2,4,5,1;
  transpose(m2);
  intvec v1=1,2,4;
  intvec v2=5,7,8;
  m1=v1,v2;         // fill m1 with v1 and v2
  m1;
  trace(m1*m2);
tst_status(1);$
