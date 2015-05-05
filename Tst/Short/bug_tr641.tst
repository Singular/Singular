LIB "tst.lib";
tst_init();

LIB "primdec.lib";
ring rng = 0,(x,y,z),dp;
ideal I = 21*x^2*z+21*z^2,7*y^2-6*y,-5*x*y^2-29*y*z+30*z^2;
def ma1 = minAssGTZ (I);
def ma2 =  minAssGTZ(I,"noFacstd");  //wrong?
ma1;
ma2;
idealsEqual( intersect(ma1[1],ma1[2],ma1[3]), std(radical(I)) );
idealsEqual( intersect(ma2[1],ma2[2] ), std(radical(I)) );

tst_status(1);$
