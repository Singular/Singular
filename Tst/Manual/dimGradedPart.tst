LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
ring R=0,(x,y,z),dp;
module M=maxideal(3);
// assign compatible weight vector (here: 0)
homog(M);
// compute dimension of graded pieces of R/<x,y,z>^3 :
dimGradedPart(M,0);
dimGradedPart(M,1);
dimGradedPart(M,2);
dimGradedPart(M,3);
// shift grading:
attrib(M,"isHomog",intvec(2));
dimGradedPart(M,2);
tst_status(1);$
