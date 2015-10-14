LIB "tst.lib";
tst_init();

// \n within argument list, next argument is "alias":

proc p1(def a1, alias def a2) {}
proc p2(def a1,
     alias def a2) {}
int n1 = 1;
int n2 = 2;
p1(n1, n2);   // this works
p2(n1, n2);

tst_status(1);$
