LIB "tst.lib";
tst_init();
tst_ignore("not provided yet");
LIB "ringutils.lib";

testHasNamePrefixConflict();
testGetNewVariableName();
testGetNewRingVariableName();


tst_status(1);$

