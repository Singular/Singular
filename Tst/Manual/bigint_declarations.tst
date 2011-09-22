LIB "tst.lib"; tst_init();
  bigint i = 42;
  ring r=0,x,dp;
  number n=2;
  bigint j = i + bigint(n)^50; j;
tst_status(1);$
