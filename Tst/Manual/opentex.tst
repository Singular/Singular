LIB "tst.lib"; tst_init();
LIB "latex.lib";
opentex("exmpl");
texobj("exmpl","hello");
closetex("exmpl");
tst_status(1);$
