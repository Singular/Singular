LIB "tst.lib"; tst_init();

option("redSB");
LIB("primdecint.lib");
proc stdZW(I)
{ return(  Primdecint::stdZ(I) );
}
ring rng = integer,(x, y, z, v),dp;

ideal Q =  8*y-2*z-5*v,
 2*x*z+5*x*v+2*z-3*v,
 x*y-7*y+2*z+4*v,
 z*v^2-2*y^2,
 5*x*v^2-3*v^2+4*y,
 4*y*z*v-z^2*v-5*y^2,
 x*z*v+z*v-2*y,
 5*x^2*v+2*x*v-3*v+4,
 x*z^2+z^2-4*z*v+5*y,
 x^2*z-5*x*v-z+3*v-2;
 Q = std(Q); 
qring q = Q;

ideal I = 2, v,y^2;
ideal J = v,2*z,2*y,y*v,y^2;

ideal IJ = intersect(I,J);
ideal stdIJ = std(IJ);
stdIJ;
ideal stdIJZ = stdZW(IJ);
stdIJZ;
int k;
 for(k=1; k <= ncols(stdIJ); k++)
{
 if(leadcoef(stdIJ[k]) < 0)
 {
    ERROR(" Warning: negative leadcoef occured");
 }
}

tst_status(1);$
