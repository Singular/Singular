//
// solve_s.tst - short tests for 

LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: solve_l.tst,v 1.4 2005-05-03 13:02:59 Singular Exp $");

LIB "solve.lib";

////////////////////////////

tst_ignore( "ring rs1 = 0,(x,y,z),lp;" );
ring rs1 = 0,(h,x,y,z),lp;

poly f0= x+y+z+1;
poly f1= x^2 + 6*x + 3*y + 6*z - 4;
poly f2= y^2 + 2*x -7*y + 5 + 2*z;
poly f3= x^2 + y^2 + z^2 - 1;
ideal i=f1,f2,f3;
ideal im=f0,f1,f2,f3;

def A=ures_solve(homog(i,h),1);
setring A; SOL;
kill A;
setring rs1;
mp_res_mat(homog(im,h),1);

tst_ignore( "ring rs2 = 0,(x,y,z),lp;" );
ring rs2 = 0,(x,y,z),lp;

poly f0= x+y+z;
poly f1= x^2 + 6*x + 3*y + 6*z - 4;
poly f2= y^2 + 2*x -7*y + 5 + 2*z;
poly f3= x^2 + y^2 + z^2 - 1;
ideal i=f1,f2,f3;
ideal im=f0,f1,f2,f3;

def A=ures_solve(i);
setring A; SOL;
kill A;
setring rs2;
mp_res_mat(im);

def rinC=fglm_solve(i,30);
setring rinC;
rlist;

setring rs2;
kill rinC;
def rinC=triangL_solve(i,30);
setring rinC; rlist;

setring rs2;
kill rinC;
def rinC=triangLf_solve(i,30);
setring rinC; rlist;

setring rs2;
kill rinC;
def rinC=triangM_solve(i,30);
setring rinC; rlist;
kill rinC;

////////////////////////////

tst_ignore( "ring rs3 = 0,(x1,x2,x3),lp;" );
ring rs3 = 0,(x0,x1,x2,x3),lp;

poly f0= x1+x2+x3+1;
poly f1= -1 + 6*x1^2 - 4*x1*x2 + 3*x2^2 - 8*x1*x3 + 10*x2*x3 + 13*x3^2;
poly f2= -2*x1 - 2*x1^2 + x2 - 2*x2^2 + 3*x1 - 4*x1*x3 - 4*x2*x3 - 4*x3^2;
poly f3= x1 - 5*x1^2 - x2 + 2*x1*x2 - 2*x2^2 + 8*x1*x3 - 10*x2*x3 - 13*x3^2;
ideal i=f1,f2,f3;
ideal im=f0,f1,f2,f3;

def A=ures_solve(homog(i,x0),1);
setring A; SOL;
kill A;
setring rs3;
mp_res_mat(homog(im,x0),1);

tst_ignore( "ring rs4 = 0,(x1,x2,x3),lp;" );
ring rs4 = 0,(x1,x2,x3),lp;

poly f0= x1+x2+x3;
poly f1= -1 + 6*x1^2 - 4*x1*x2 + 3*x2^2 - 8*x1*x3 + 10*x2*x3 + 13*x3^2;
poly f2= -2*x1 - 2*x1^2 + x2 - 2*x2^2 + 3*x1 - 4*x1*x3 - 4*x2*x3 - 4*x3^2;
poly f3= x1 - 5*x1^2 - x2 + 2*x1*x2 - 2*x2^2 + 8*x1*x3 - 10*x2*x3 - 13*x3^2;
ideal i=f1,f2,f3;
ideal im=f0,f1,f2,f3;

def A=ures_solve(i);
setring A; SOL;
kill A;
setring rs4;
mp_res_mat(im);

def rinC=fglm_solve(i,30);
setring rinC; rlist;

setring rs4;
kill rinC;
def rinC=triangL_solve(i,30);
setring rinC;rlist;

setring rs4;
kill rinC;
def rinC=triangLf_solve(i,30);
setring rinC;rlist;

setring rs4;
kill rinC;
def rinC=triangM_solve(i,30);
setring rinC;rlist;

////////////////////////////

tst_ignore( "ring rs6= 0,(x,y),lp;" );
ring rs6= 0,(x,y),lp;
poly f = 15x5 + x3 + x2 - 10;
laguerre_solve(f);

laguerre_solve(f,10);

tst_ignore( "ring rsc= (real,20,I),x,lp;" );
ring rsc= (real,20,I),x,lp;
poly f = (15.4+I*5)*x^5 + (25.0e-2+I*2)*x^3 + x2 - 10*I;
laguerre_solve(f);

////////////////////////////

ring rs7 = 0,(x),lp;
ideal v=16,0,11376,1046880,85949136;
interpolate( 3, v, 4 );

ring rs8 = 0,(x,y),dp;
ideal p = 2,3;
ideal v= 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16;
poly ip= interpolate( p,v,3 );
ip;

////////////////////////////

tst_status(1);$

