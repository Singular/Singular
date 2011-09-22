LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y),Dp;
ideal F = x,y,x+y;
printlevel = 0;
// *1* let us compute the B ideal
def A = annfsBMI(F);    setring A;
LD; // annihilator
BS; // Bernstein-Sato ideal
// *2* now, let us compute B-Sigma ideal
setring r;
def Sigma = annfsBMI(F,0,-1); setring Sigma;
print(matrix(lead(LD))); // compact form of leading
//  monomials from the annihilator
BS; // Bernstein-Sato B-Sigma ideal: it is principal, 
// so factors and multiplicities are returned
// *3* and now, let us compute B-i ideal 
setring r; 
def Bi = annfsBMI(F,0,3); // that is F[3]=x+y is taken
setring Bi;
print(matrix(lead(LD)));   // compact form of leading
//  monomials from the annihilator
BS; // the B_3 ideal: it is principal, so factors
// and multiplicities are returned
tst_status(1);$
