LIB "tst.lib"; tst_init();
  string s="abcde";
  s[2];
  s[3,2];
  ">>"+s[1,10]+"<<";
  s[2]="BC"; s;
  intvec v=1,3,5;
  s=s[v]; s;
  s="654321"; s=s[3..5]; s;
tst_status(1);$
