LIB "tst.lib"; tst_init();
LIB "classify.lib";
ring r=0,(x,y,z),(c,ds);
init_debug(0);
singularity("E[6k]",6);
singularity("T[k,r,s]", 3, 7, 5);
poly f=y;
singularity("J[k,r]", 4, 0, 0, f);
tst_status(1);$
