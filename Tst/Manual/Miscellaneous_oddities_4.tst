LIB "tst.lib"; tst_init();
package K;
string K::varok;
string K::donotwork(1);
int K::i(1..3);
// Toplevel does not contain i(1..3)
listvar();
// i(1..3) are stored in Package 'K'
listvar(K);
tst_status(1);$
