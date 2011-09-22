LIB "tst.lib"; tst_init();
  string s = "one-word";
  s[2,6];     // a substring starting at the second char
  size(_);
  intvec v = 2,6;
  s[v];      // the second and the sixth char
  string st = s[v];  // stick together by an assignment
  st;
  size(_);
  v = 2,6,8;
  s[v];
tst_status(1);$
