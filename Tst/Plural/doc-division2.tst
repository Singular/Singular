LIB "tst.lib";
tst_init();
LIB "dmod.lib";
ring r = 0,(x,y),dp;
poly f = x^3+xy;
def S = Sannfs(f); setring S; // compute the annihilator of f^s
LD; // is not a Groebner basis yet!
poly f = imap(r,f);
poly P = f*Dx-s*diff(f,x);
ideal I = LD, f; // consider a bigger ideal
module A = LD*gen(1), I*gen(2); module B = slimgb(A);
vector V = [P,(s+1)^2]; NF(V,B); //0
list LV = division(V,A);
vector W = [1,s^2]; NF(W,B); //!=0
list LW = division(W,A);
matrix M = matrix(W); matrix N = matrix(A);
matrix T = matrix(LW[1]); matrix R = matrix(LW[2]); matrix U  = matrix(LW[3]);
// test:
transpose(U)*transpose(M) - transpose(T)*transpose(N) - transpose(R); // must be 0
vector W2 = [s^2,s^2]; NF(W2,B); //!=0
// matrix to matrix
matrix M2[2][2] = 1,s^2,s^2,1;
list LW2 = division(M2,A);
kill M,N;
matrix M = matrix(M2); matrix N = matrix(A);
kill T,R,U;
matrix T = matrix(LW2[1]); matrix R = matrix(LW2[2]); matrix U  = matrix(LW2[3]);
transpose(U)*transpose(M) - transpose(T)*transpose(N) - transpose(R); // must be 0
tst_status(1);$
