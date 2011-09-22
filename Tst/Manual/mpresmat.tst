LIB "tst.lib"; tst_init();
  ring rsq=(0,s,t,u),(x,y),lp;
  ideal i=s+tx+uy,x2+y2-10,x2+xy+2y2-16;
  module m=mpresmat(i,0);
  print(m);
tst_status(1);$
