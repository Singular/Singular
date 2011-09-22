LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
ring R=0,(x,y,z),dp;
module M=maxideal(3);
homog(M);
// compute presentation matrix for truncated module (R/<x,y,z>^3)_(>=2)
module M2=truncate(M,2);
print(M2);
dimGradedPart(M2,1);
dimGradedPart(M2,2);
// this should coincide with:
dimGradedPart(M,2);
// shift grading by 1:
intvec v=1;
attrib(M,"isHomog",v);
M2=truncate(M,2);
print(M2);
dimGradedPart(M2,3);
tst_status(1);$
