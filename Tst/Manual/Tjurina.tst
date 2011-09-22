LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 0,(x,y,z),ds;
poly f     = x5+y6+z7+xyz;        // singularity T[5,6,7]
list T     = Tjurina(f,"");
show(T[1]);                       // Tjurina number, should be 16
show(T[2]);                       // basis of miniversal deformation
show(T[3]);                       // SB of Tjurina ideal
show(T[4]); "";                   // Tjurina ideal
ideal j    = x2+y2+z2,x2+2y2+3z2;
show(kbase(Tjurina(j)));          // basis of miniversal deformation
hilb(Tjurina(j));                 // Hilbert series of Tjurina module
printlevel = p;
tst_status(1);$
