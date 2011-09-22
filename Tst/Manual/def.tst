LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  poly f = x;
  def s=basering;
  setring s;
  nameof(basering);
  listvar();
  poly g = y;
  kill f;
  listvar(r);
  ring t=32003,(u,w),dp;
  def rt=r+t;
  rt;
tst_status(1);$
