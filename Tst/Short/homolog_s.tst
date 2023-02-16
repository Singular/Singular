//homolog_s.tst
//short tests for homolog.lib
//---------------------------
LIB "tst.lib";
tst_init();
LIB "homolog.lib";
example cup;
example cupproduct;
example depth;
example Ext_R;
example Ext;
example fitting;
example flatteningStrat;
example Hom;
example homology;
example isCM;
example isFlat;
example isLocallyFree;
example isReg;
example hom_kernel;
example kohom;
example kontrahom;
example KoszulHomology;
example tensorMod;
example Tor;
ring r;
ideal i=x,0,z;
print(KoszulMap(i,2));
tst_status(1);$
