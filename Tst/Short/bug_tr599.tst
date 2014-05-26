LIB "tst.lib";
tst_init();

// assign expr_list to def: should yield an error
def l1=1,2;

list l2;
l2[1]=1,2;
l2;

tst_status(1);$
