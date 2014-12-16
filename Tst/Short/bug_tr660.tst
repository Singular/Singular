LIB "tst.lib";
tst_init();

// primary decomposition incomplete using given ordering
LIB"primdec.lib";
option(redTail);
option(redSB);
ring rng = (0),(x,y,z,w),lp;
ideal I =
1003/1073*x*w^2+3856/1601*y^2-6193/5747*y*z+10820/19117*w,
-2779/1010*x+1505/1007*y^2,
9233/8945*z*w-6187/6861*w^2;

list L1 = primdecGTZ (I);
list L2 = primdecSY (I,0);
L1;
L2;

def pr2 = primdecSY( L2[2][1], 0  );
pr2;
size(pr2);

tst_status(1);$
