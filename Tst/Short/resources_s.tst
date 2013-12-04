LIB "tst.lib";
tst_init();

LIB "resources.lib";

setcores(4);
getcores();
addcores(-2);
getcores();
addcores(2);
getcores();
setcores(1);
getcores();

tst_status(1);$
