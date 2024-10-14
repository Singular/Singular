LIB "tst.lib"; tst_init();
LIB "sresforextalg.lib";
// single syzygy computation over the exterior algebra
ring r1 = (0,q),(x,y,z),dp;
def e = Exterior(); setring e;
vector f1 = [x,y,0,z,0,0];
vector f2 = [0,x,y,0,z,0];
vector f3 = [0,0,0,x,y,z];
module m1 = f1,f2,f3;
m1 = std(m1);
schreyerSyzExt(m1);
//Schreyer resolution over teh Exterior Algebra
ring r = 0,(x,y,z),dp;
def e2 = Exterior(); setring e2;
vector f1 = [x,y,0,z,0,0];
vector f2 = [0,x,y,0,z,0];
vector f3 = [0,0,0,x,y,z];
module m2 = f1,f2,f3;
m2 = std(m2);
sresExt(m2,4);
//Minimal Schreyer resolution over the exterior algebra
ring r = 0,(x,y,z),dp;
def E = Exterior();
vector f1 = [x,y,0,z,0,0];
vector f2 = [0,x,y,0,z,0];
vector f3 = [0,0,0,x,y,z];
module m3 = f1,f2,f3;
mresExt(m3,4);
// cohomology of structure sheaf on P^4:
//-------------------------------------------
ring r=0,x(1..5),dp;
module M=0;
def A=sheafCohBGG_sres(M,-9,4);
displayCohom(A,-9,4,4);
// cohomology of cotangential bundle on P^3:
//-------------------------------------------
ring R=0,(x,y,z,u),dp;
resolution T1=mres(maxideal(1),0);
module M=T1[3];
intvec v=2,2,2,2,2,2;
attrib(M,"isHomog",v);
def B=sheafCohBGGsres(M,-8,4);
displayCohom(B,-8,4,3);
tst_status(1);$