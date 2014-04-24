LIB "tst.lib";
tst_init();

// non-prime result for small char.
LIB"primdec.lib";
system("random",1397912804);

ring rng = 7,(x,y,z),dp;
ideal I = -y3+2x2-1,-z3,-x2y-1;

def L1 = primdecGTZ(I);
def L2 = primdecGTZ(I);
L1;
L2;


tst_status(1);$
