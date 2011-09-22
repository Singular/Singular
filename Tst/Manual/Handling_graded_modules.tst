LIB "tst.lib"; tst_init();
ring R = 0, (w,x,y,z), dp;
module I = [-x,0,-z2,0,y2z], [0,-x,-yz,0,y3], [-w,0,0,yz,-z3],
           [0,-w,0,y2,-yz2], [0,-1,-w,0,xz], [0,-w,0,y2,-yz2],
           [x2,-y2,-wy2+xz2];
print(I);

// (1) Check on degrees:
// =====================
attrib(I,"isHomog"); // attribute not set => empty output
homog(I);
attrib(I,"isHomog");

print(betti(I,0),"betti");  // read degrees from Betti diagram

// (2) Shift degrees:
// ==================
def J=I;
intvec DV = 0,0,-1,-1,-2;
attrib(J,"isHomog",DV);    // assign new weight vector
attrib(J,"isHomog");
print(betti(J,0),"betti");

intmat bettiI=betti(I,0);  // degree corresponding to first non-zero row
                           // of Betti diagram is accessible via
                           // attribute "rowShift"
attrib(bettiI);
intmat bettiJ=betti(J,0);
attrib(bettiJ);

// (3) Graded free resolutions:
// ============================
resolution resJ = mres(J,0);
attrib(resJ);
print(betti(resJ),"betti");
attrib(betti(resJ));
tst_status(1);$
