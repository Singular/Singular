LIB "tst.lib"; tst_init();

ring r;
module m=[1],[x],[0,z],[0,0,1];
smatrix t;
matrix mm;
prune_map (m,t);
prune_map (m,1);
prune_map (m,mm);
tst_status(1);$
