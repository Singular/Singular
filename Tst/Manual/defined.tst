LIB "tst.lib"; tst_init();
  ring r=(0,t),(x,y),dp;
  matrix m[5][6]=x,y,1,2,0,x+y;
  defined(mm);
  defined(r) and defined(m);
  defined(m)==voice;   // m is defined in the current level
  defined(x);
  defined(z);
  defined("z");
  defined(t);
  defined(42);
tst_status(1);$
