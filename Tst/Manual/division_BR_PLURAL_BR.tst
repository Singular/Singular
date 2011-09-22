LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y),dp;
poly f = x^3+xy;
def S = Sannfs(f); setring S; // compute the annihilator of f^s
LD; // is not a Groebner basis yet!
poly f = imap(r,f);
poly P = f*Dx-s*diff(f,x);
division(P,LD); // so P is in the ideal via the cofactors in _[1]
ideal I = LD, f; // consider a bigger ideal
list L = division(s^2, I); // the normal form is -2s-1
L;
// now we show that the formula above holds
matrix M[1][1] = s^2; matrix N = matrix(I);
matrix T = matrix(L[1]); matrix R = matrix(L[2]); matrix U  = matrix(L[3]);
// the formula must return zero:
transpose(U)*transpose(M) - transpose(T)*transpose(N) - transpose(R);
tst_status(1);$
