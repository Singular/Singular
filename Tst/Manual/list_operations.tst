LIB "tst.lib"; tst_init();
  list l1 = 1,"hello",list(-1,1);
  list l2 = list(1,5,7);
  l1 + l2;             // a new list
  l2 = delete(l2, 2);  // delete 2nd entry
  l2;
tst_status(1);$
