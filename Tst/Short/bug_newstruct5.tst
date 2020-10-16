LIB "tst.lib";
tst_init();

// access objects from other rings

newstruct("out_struct", "ideal outputideal");
ring R;
ideal I = 123x4y5+12z3,18y7+3x2y2z2;
out_struct ot;
ot.outputideal = I;
ring S = 0,a,dp;
ideal J = ot.outputideal;

setring R;
ideal J = ot.outputideal;

tst_status(1);$

