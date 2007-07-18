LIB "tst.lib";
tst_init();

// ========================== example 1.9.20  ==================================

LIB "ncalg.lib"; 
// load the library with the definition of U((sl_2)
def A = makeUsl(2); // set up U(sl_2)
setring A;
option(redSB);  
option(redTail); // we wish to compute reduced bases
ideal I = e2,f;
ideal LI = std(I);
LI;

tst_status(1);$

