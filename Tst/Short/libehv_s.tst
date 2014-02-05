LIB "tst.lib";
tst_init();


LIB("ehv.lib");

proc testEhv()
{
  Ehv::testPrimDecsAreEquivalent();
}
testEhv();


tst_status(1); $;

