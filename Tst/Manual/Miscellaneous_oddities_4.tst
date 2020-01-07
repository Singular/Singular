LIB "tst.lib"; tst_init();
package K;
string varok; exportto(K,varok);
int i(1..3); exportto(K,i(1..3));
// Toplevel does not contain i(1..3)
listvar();
// i(1..3) are stored in Package 'K'
listvar(K);
tst_status(1);$
