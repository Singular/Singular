LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
// cohomology of structure sheaf on P^4:
//-------------------------------------------
ring r=0,x(1..5),dp;
module M=0;
def A=sheafCohBGG(M,-9,4);
displayCohom(A,-9,4,4);
// cohomology of cotangential bundle on P^3:
//-------------------------------------------
ring R=0,(x,y,z,u),dp;
resolution T1=mres(maxideal(1),0);
module M=T1[3];
intvec v=2,2,2,2,2,2;
attrib(M,"isHomog",v);
def B=sheafCohBGG(M,-8,4);
displayCohom(B,-8,4,3);
tst_status(1);$
