LIB "tst.lib";
tst_init();

// internal factorization in Z/2[...] and (Z/2,a)[...]

LIB"primdec.lib";
ring r=2,(x,y,z,w),dp;
ideal I=w2+xy3+yz3+zx7;
ideal JI=jacob(I),I;
int i;
for(i=0;i<100;i++)
{
def huhu=primdecGTZ(JI);
kill huhu;
i;
}


tst_status(1);$
