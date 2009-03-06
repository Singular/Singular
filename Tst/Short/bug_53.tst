LIB "tst.lib";
tst_init();

// bug in interred, giving 7 sol. instead of 21:
// trac ticket 102
LIB "solve.lib"; 
ring r=0,(x,y,z,z(1),z(2)),dp; poly
N1=-x4+x3*y-x2*y2+x*y3-x2*z2+x*y*z2+x3-x2*y+x*y2-y3+x*z2-y*z2+16*x2-16*x*y-16*x+16*y+10*z(1)+7*z(2); poly
N2=-x3*y-x*y3+x2*y*z+y3*z-x*y*z2+y*z3+2*x3+2*x*y2-2*x2*z-2*y2*z+2*x*z2-2*z3+16*x*y-16*y*z-32*x+32*z-7*z(1)+2*z(2);
poly N3=x4*z-x3*y*z+x2*y2*z-x*y3*z-x3*z2+x2*y*z2-x*y2*z2+y3*z2+x2*z3-x*y*z3-x*z4+y*z4-3*x4+3*x3*y-3*x2*y2+3*x*y3+3*x3*z-3*x2*y*z+3*x*y2*z-3*y3*z-3*x2*z2+3*x*y*z2+3*x*z3-3*y*z3-16*x2*z+16*x*y*z+16*x*z2-16*y*z2+48*x2-48*x*y-48*x*z+48*y*z+z(1)+5*z(2);
poly N4=-3*x+3*y+2*z+z(1)-8;
poly N5=6*x-8*y+9*z+z(2)+2;
ideal N=N1,N2,N3,N4,N5;
def s = solve(N);
setring s;
SOL; 

tst_status(1);$
