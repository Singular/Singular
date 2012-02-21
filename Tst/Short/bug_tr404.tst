LIB "tst.lib";
tst_init();

// alias stuff
proc dummy1(alias list l) { int i = l[1]; i; };
proc dummy2(alias list l) { int i = l[1][1]; i; };
list l1 = list(1, 2);
list l2 = list(list(1), list(2));
dummy1(l1);
dummy2(l2);
proc adummy1(int a, int b) { return("works"); };
proc adummy2(alias int a, int b) { return("works"); };
proc adummy3(int a, alias int b) { return("works"); };
proc adummy4(alias int a, alias int b) { return("works"); };
int a, b;
adummy1(a, b);
adummy2(a, b);
adummy3(a, b);
adummy4(a, b);

tst_status(1);$
