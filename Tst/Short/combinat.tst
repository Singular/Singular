LIB "tst.lib";
tst_init();

LIB "combinat.lib";
printlevel=0;

  list L = 1,2,3,4,5;
  sublists(L);

  ring R=(0,x1,x2,x3,x4),(q),dp;
  permute(list(x1,x2,x3,x4));

  partitions(3,7);

  intersectLists(list(1,1,2,2,3),list(2,3,3,4));
  intersectLists(list(1,1,2,2,3),list(1,1,2,2,3));

  member(1,list(1,4,5));

tst_status(1);$
