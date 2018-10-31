LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),(dp(1),dp(2)); //  ring r = 0,(x,y,z),(a(3,0,2), dp(2));
module M = [-1,x,y],[-7,y,y],[3,x,x]; // stands for free poly -xy - 7yy - 3xx
module N = [1,x,y,x],[-1,y,x,y]; // stands for free poly xyx - yxy
list L; L[1] = M; L[2] = N; // list of modules stands for an ideal in free algebra
def U = freeGBasis(L,5); // 5 is the degree bound
U;
tst_status(1);$
