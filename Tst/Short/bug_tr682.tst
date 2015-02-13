LIB "tst.lib";
tst_init();

// generic position not found
LIB"primdec.lib";

proc anotherIndirection()
{
    ring rng = 18457,(x,y,z,w),dp;
    ideal I = -172*x^2+57*x*y*z-36*z^2*w,146*x*y^2+129*x*z;
    minAssGTZ (I,"GTZ");
}
anotherIndirection(); //  ? nesting too deep

tst_status(1);$

