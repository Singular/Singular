LIB "tst.lib"; tst_init();
LIB "control.lib";
// Flexible Rod
ring A = 0,(D1, D2), (c,dp);
module R= [D1, -D1*D2, -1], [2*D1*D2, -D1-D1*D2^2, 0];
module RR = transpose(R);
list L = control(RR);
// here, we have the annihilator:
ideal LAnn = D1; // = L[10]
module Tr  = findTorsion(RR,LAnn);
print(RR);  // the module itself
print(Tr); // generators of the torsion submodule
tst_status(1);$
