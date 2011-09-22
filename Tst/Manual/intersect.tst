LIB "tst.lib"; tst_init();
  ring R=0,(x,y),dp;
  ideal i=x;
  ideal j=y;
  intersect(i,j);
  ring r=181,(x,y,z),(c,ls);
  ideal id1=maxideal(3);
  ideal id2=x2+xyz,y2-z3y,z3+y5xz;
  ideal id3=intersect(id1,id2,ideal(x,y));
  id3;
tst_status(1);$
