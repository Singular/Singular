  timer = 1; // The time of each command is printed
  int t=timer;
  ring r=0,(x,y,z),dp;
  poly p=(x+2y+3z+4xy+5xz+6yz)^20;
  // timer as int_expression:
  timer-t;  // returns the time used in sec
            // since t was set to timer
LIB "tst.lib";tst_status(1);$
