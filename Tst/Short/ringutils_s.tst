LIB "tst.lib";
tst_init();


LIB("ring.lib");

proc testRingUtils()
{
  Ring::testIsQuotientRing();
  Ring::testHasIntegerCoefficientRing();
}
testRingUtils();


tst_status(1);
quit;

