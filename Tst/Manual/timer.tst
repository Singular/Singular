LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  timer = 0;
  poly p=(x+2y+3z+4xy+5xz+6yz)^20;
  // timer as int_expression:
  int t = timer;
  tst_ignore(string(t)); // yields the time in ticks-per-sec (default 1)
                     // since t was initialized by timer
  int tps=system("--ticks-per-sec");
  tst_ignore(string(t div tps)); // yields the time in seconds truncated to int
  system("--ticks-per-sec",1000); // set timer resolution to ms
  timer=0;
  p=(x+2y+3z+4xy+5xz+6yz)^20;
  tst_ignore(string(timer));  // time in ms
tst_status(1);$
