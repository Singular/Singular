LIB "tst.lib"; tst_init();
  ring R= 0,(x,y,z,u),dp;
  ideal i=-3zu+y2-2x+2,
          -3x2u-4yz-6xz+2y2+3xy,
          -3z2u-xu+y2z+y;
  print(char_series(i));
tst_status(1);$
