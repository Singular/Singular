LIB "tst.lib"; tst_init();
  string("1+1=", 2);
  string(intvec(1,2,3,4));
  string(intmat(intvec(1,2,3,4), 2, 2));
  ring r;
  string(r);
  string(ideal(x,y));
  qring R = std(ideal(x,y));
  string(R);
  map phi = r, ideal(x,z);
  string(phi);
  list l;
  string(l);
  l[3] = 1;
  string(l); // notice that l[1],l[2] are omitted
  l[2] = l;
  l;
  string(l); // notice that lists of list is flattened
  l[1] = intvec(1,2,3);
  l;
  string(l); // notice that intvec elements are not distinguishable
tst_status(1);$
