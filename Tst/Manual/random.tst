LIB "tst.lib"; tst_init();
  random(1,1000);
  random(1,2,3);
  system("random",210);  // start random generator with 210
  random(-1000,1000);
  random(-1000,1000);
  system("random",210);
  random(-1000,1000);    // the same random values again
tst_status(1);$
