LIB "tst.lib"; tst_init();
// Note: 11*13*17*100*200*2000*503*1111*222222
// returns a machine integer:
11*13*17*100*200*2000*503*1111*222222;
// using the type cast number for a greater allowed range
bigint(11)*13*17*100*200*2000*503*1111*222222;
tst_status(1);$
