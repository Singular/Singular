LIB "tst.lib";
tst_init();

LIB "normal.lib";
ring r = 2, (v, u, z, y, x), dp;
ideal I = z3+zyx+y3x2+y2x3, uyx+z2,uz+z+y2x+yx2, u2+u+zy+zx, v3+vux+vz2+vzyx+vzx
+uz3+uz2y+z3+z2yx2;
r;
I;
list L = normalC(I, "isPrim");
L;
kill r;
//----------------------------------------------
ring r = 2, (v, u, z, y, x), dp;
ideal I = z3+zyx+y3x2+y2x3, uyx+z2,uz+z+y2x+yx2, u2+u+zy+zx, v3+vux+vz2+vzyx+vzx
+uz3+uz2y+z3+z2yx2;
r;
I;
list L = normal(I, "useRing", "isPrim");
L;
kill r;
//---------------------------------------------
ring r=2,(v,u,z,y,x),dp;
ideal i = z3+zyx+y3x2+y2x3, uyx+z2,uz+z+y2x+yx2, u2+u+zy+zx,
v3+vux+vz2+vzyx+vzx+uz3+uz2y+z3+z2yx2;
equidim(i);

tst_status(1);$
