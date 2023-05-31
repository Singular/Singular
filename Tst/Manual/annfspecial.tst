LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y),dp;
poly F = x3-y2;
bernsteinBM(F); // the roots of Bernstein-Sato poly: -7/6, -1, -5/6
// *** first example: generic root
def A = annfspecial(F,-5/6);
setring A; print(annfalpha); kill A; setring r;
// *** second example:  exceptional root since its distance to -1 is integer 2
def A = annfspecial(F,1);
setring A;  print(annfalpha); kill A; setring r;
// *** third example: exceptional root since its distance to -5/6 is integer 1
def A = annfspecial(F,1/6);
setring A;  print(annfalpha); kill A;
tst_status(1);$
