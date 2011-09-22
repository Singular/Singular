LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="alpha","beta","gamma";
setinitials(V,D,P);////does not show the ring, since there is no output
basering;///does show the ring
tst_status(1);$
