LIB "tst.lib";
LIB "hodge.lib";
tst_init();
ring MYRINGNAME=0,(x,y,z,w),dp;
poly MYPOLYNAME=x*w-y*z;
def Ra=hodgeIdeals(MYPOLYNAME, 3);
setring Ra; hodge;
kill MYRINGNAME;
tst_status(1);$
