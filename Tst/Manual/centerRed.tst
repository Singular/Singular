LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=z;
def A = nc_algebra(1,D); setring A; // it is a Heisenberg algebra
// find a basis of the vector space of
// central elements of degree <= 3:
ideal VSZ = centerVS(3);
// There should be 3 degrees of z.
VSZ;
inCenter(VSZ); // check the result
// find "minimal" central elements of degree <= 3
ideal SAZ = centerRed(3);
// Only 'z' must be computed
SAZ;
inCenter(SAZ); // check the result
tst_status(1);$
