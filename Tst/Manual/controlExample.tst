LIB "tst.lib"; tst_init();
LIB "control.lib";
controlExample("show");   // let us see all available examples:
def B = controlExample("TwoPendula"); // let us set up a particular example
setring B;
print(R);
tst_status(1);$
