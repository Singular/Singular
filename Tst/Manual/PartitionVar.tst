LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);////does not show the ring, since there is no output
basering;///does show the ring
poly f=t**3*cx**2-cy**2*dt+i**3*sx;
PartitionVar(f,1); ////i is the first variable
tst_status(1);$
