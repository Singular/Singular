  ring r=32003,(x,y,z),dp;
  poly f=x5+5x4y+10x2y3+y5;
  matrix m=coef(f,y);
  print(m);
  f=x^20+xy+x2y;
  print(coef(f,xy));
  vector v=[f,zy+77+xy];
  print(v);
  matrix mc;matrix mm;
  coef(v,y,mc,mm);
  print(mc);
  print(mm);
LIB "tst.lib";tst_status(1);$
