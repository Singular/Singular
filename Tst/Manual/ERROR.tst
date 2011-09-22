LIB "tst.lib"; tst_init();
int i=1;
proc myError() {ERROR("Need to leave now");i=2;}
myError();
i;
tst_status(1);$
