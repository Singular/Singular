LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);////does not show the ring, as there is  no output
basering;///does show the ring
poly f=t**3*cx**2-cy**2*dt+i**3*sx;
f;
VarToPar(f);
tst_status(1);$
