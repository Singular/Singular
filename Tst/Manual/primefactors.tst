LIB "tst.lib"; tst_init();
  bigint n = bigint(7)^12 * bigint(37)^6 * 121;
  primefactors(n);
  primefactors(n, 15); /* only prime factors <= 15 */
tst_status(1);$
