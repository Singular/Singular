LIB "tst.lib"; tst_init();
LIB "algebra.lib";
int p = printlevel;
ring r = 0,(a,b,c),ds;
ring s = 0,(x,y,z,u,v,w),dp;
ideal I = x-w,u2w+1,yz-v;
map phi = r,I;                    // a map from r to s:
is_injective(phi,r);              // a,b,c ---> x-w,u2w+1,yz-v
ring R = 0,(x,y,z),dp;
ideal i = x, y, x2-y3;
map phi = R,i;                    // a map from R to itself, z --> x2-y3
list l = is_injective(phi,R,"");
l[1];
def S = l[2]; setring S;
ker;
tst_status(1);$
