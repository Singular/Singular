LIB "tst.lib"; tst_init();
  ring r1 = 0,(z,y,x),ds;
  poly s1=2x5y+7x2y4+3x2yz3;
  poly s2=1x2y2z2+3z8;
  poly s3=4xy5+2x2y2z3+11x10;
  ideal i=s1,s2,s3;
  ideal j=std(i);
  reduce(3z3yx2+7y4x2+yx5+z12y2x2,j);
  reduce(3z3yx2+7y4x2+yx5+z12y2x2,j,1);
  // 4 arguments:
  ring rs=0,x,ds;
  // normalform of 1/(1+x) w.r.t. (x3) up to degree 5
  reduce(poly(1),1+x,ideal(x3),5);
tst_status(1);$
