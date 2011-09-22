LIB "tst.lib"; tst_init();
LIB "classify.lib";
init_debug();
debug_log(1,"no trace information printed");
init_debug(1);
debug_log(1,"some trace information");
init_debug(2);
debug_log(2,"nice for debugging scripts");
init_debug(0);
tst_status(1);$
