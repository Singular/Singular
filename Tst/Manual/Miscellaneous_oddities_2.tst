LIB "tst.lib"; tst_init();
  LIB "nctools.lib";
  ring r = 0,(x,y),dp;
  def S = superCommutative();
  xy == yx;
  x*y == y*x;
  x*y, y*x;
tst_status(1);$
