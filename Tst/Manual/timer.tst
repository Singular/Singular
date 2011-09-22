LIB "tst.lib"; tst_init();
  timer=1; // The time of each command is printed
  int t=timer; // initialize t by timer
  ring r=0,(x,y,z),dp;
  poly p=(x+2y+3z+4xy+5xz+6yz)^20;
  // timer as int_expression:
  t=timer-t;
  t;  // yields the time in ticks-per-sec (default 1)
      // since t was initialized by timer
  int tps=system("--ticks-per-sec");
  t/tps; // yields the time in seconds truncated to int
  timer=0;
  system("--ticks-per-sec",1000); // set timer resolution to ms
  t=timer; // initialize t by timer
  p=(x+2y+3z+4xy+5xz+6yz)^20;
  timer-t;  // time in ms
tst_status(1);$
