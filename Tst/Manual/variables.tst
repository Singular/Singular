LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  variables(2);
  variables(x+y2);
  variables(ideal(x+y2,x3y,z));
  string(variables(ideal(x+y2,x3y,z)));
tst_status(1);$
