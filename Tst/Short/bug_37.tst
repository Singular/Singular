LIB "tst.lib";
tst_init();

  list K1=list(1,2,3),list("a","b","c");
  string h=K1[2][2]; K1[2][1]=h;// ok.
  K1[2][1]=K1[2][2];// sig.11
K1;

//
tst_status(1);$
