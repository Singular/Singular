LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
ring R=0,(x,y,z,u,v),dp;
module M=maxideal(3);
homog(M);
// compute presentation matrix for truncated module (R/<x,y,z,u>^3)_(>=2)
int t=timer;
module M2t=truncate(M,2);
t = timer - t;
"// Simple truncate: ", t;
t=timer;
module M2=truncateFast(std(M),2);
t = timer - t;
"// Fast truncate: ", t;
print(M2);
"// Check: M2t == M2?: ", size(NF(M2, std(M2t))) + size(NF(M2t, std(M2)));
dimGradedPart(M2,1);
dimGradedPart(M2,2);
// this should coincide with:
dimGradedPart(M,2);
// shift grading by 1:
intvec v=1;
attrib(M,"isHomog",v);
t=timer;
M2t=truncate(M,2);
t = timer - t;
"// Simple truncate: ", t;
t=timer;
M2=truncateFast(std(M),2);
t = timer - t;
"// Fast truncate: ", t;
print(M2);
"// Check: M2t == M2?: ", size(NF(M2, std(M2t))) + size(NF(M2t, std(M2))); //?
dimGradedPart(M2,3);
tst_status(1);$
