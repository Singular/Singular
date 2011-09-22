LIB "tst.lib"; tst_init();
LIB "reesclos.lib";
ring R = 0,(x,y),dp;
ideal I = x2,xy4,y5;
list L = ReesAlgebra(I);
def Rees = L[1];       // defines the ring Rees, containing the ideal ker
setring Rees;          // passes to the ring Rees
Rees;
ker;                   // R[It] is isomorphic to Rees/ker
tst_status(1);$
