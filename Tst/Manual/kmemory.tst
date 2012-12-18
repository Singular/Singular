LIB "tst.lib"; tst_init();
LIB "general.lib";
tst_ignore(string(kmemory()));
tst_ignore(string(kmemory(1,1)));
tst_status(1);$
