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
example fastTensorMod;
ring tensor_check=0,(x,y,z),dp;
matrix TM[3][3]=1,2,3,4,5,6,7,8,9;
matrix TN[2][2]=x,y,0,z;
print(fastTensorMod(TM,TN)-tensorMod(TM,TN));
example Tor;
ring r;
ideal i=x,0,z;
print(KoszulMap(i,2));
tst_status(1);$
