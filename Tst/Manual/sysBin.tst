LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
// [31,16,7] quadratic residue code
list l=1,5,7,9,19,25;
// we do not need even synromes here
def A=sysBin(3,l,31);
setring A;
print(bin);
tst_status(1);$
