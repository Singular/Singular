LIB "tst.lib"; tst_init();
LIB "latex.lib";
opentex("exmpl");
texobj("exmpl","{\\large \\bf hello}");
closetex("exmpl");
tst_status(1);$
