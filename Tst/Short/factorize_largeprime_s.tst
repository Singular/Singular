LIB "tst.lib";
tst_init();

LIB "primdec.lib";

ring r=(integer,1000000007),(x,y,z),(dp(1),lp(2));
poly f=(x+2*y+3)*(x*y+z+5)*(x2+y*z+7);
factorize(f);

poly g=(x+y+1)^2*(x*z+3)^3;
sqrfree(g);

ideal I=(x+2*y+3)*(x*y+z+5),z;
minAssGTZ(I);

ring r0=(integer,1000000007),(x,y,z),lp;
list rl=ringlist(r0);
rl[1]=list(rl[1],list(z),list(list("lp",1)),ideal(0));
rl[2]=list(x,y);
rl[3]=list(list("lp",1:2),list("C",0));
def R=ring(rl);
setring R;

poly ft=(x+2*y+3)*(x*y+z+5);
factorize(ft);

poly fs=(x+y+1)^2*(x*z+3)^3;
sqrfree(fs);

tst_status(1);$
