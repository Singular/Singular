LIB "tst.lib"; tst_init();
  system("--quiet");    // if ``quiet'' 1, otherwise 0
  system("--min-time"); // minimal reported time
  system("--random");   // seed of the random generator
tst_status(1);$
