LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  univariate(x2+1);
  univariate(x2+y+1);
  univariate(1);
  univariate(var(2));
  var(univariate(z));
tst_status(1);$
