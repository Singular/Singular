LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z),Dp;
ideal I = w^3*x*y,w*x*y*z,x^2*y^2*z^2,x^2*z^4,y^3*z;
// Vasconcelos para primaria
primdecMon(I,"vp");
// Alexander dual
primdecMon(I,"add");
// label algorithm
primdecMon(I,"lr");
//slice algorithm
primdecMon(I,"sr");
tst_status(1);$
