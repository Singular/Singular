LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
ring R=0,(x,y,z,u),dp;
resolution T1=mres(maxideal(1),0);
module M=T1[3];
intvec v=2,2,2,2,2,2;
attrib(M,"isHomog",v);
dimH(0,M,2);
dimH(1,M,0);
dimH(2,M,1);
dimH(3,M,-5);

// A saturated degree slice may miss sections through H^1_m.
ring P2=0,(x,y,z),dp;
ideal irr=x,y,z;
module mPresentation=syz(irr);
attrib(mPresentation,"isHomog",intvec(1,1,1));
dimGradedPart(mPresentation,0);
dimH(0,mPresentation,0);

// Conversely H^0_m must be removed from a finite-length quotient.
module residuePresentation=irr;
attrib(residuePresentation,"isHomog",intvec(0));
dimGradedPart(residuePresentation,0);
dimH(0,residuePresentation,0);

// P^0 exercises the Ext^0 boundary in the H^1_m contribution.
ring P0=0,(t),dp;
module shiftedFree=0;
attrib(shiftedFree,"isHomog",intvec(-2));
dimH(0,shiftedFree,0);
tst_status(1);$
