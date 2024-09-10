LIB "tst.lib"; tst_init();
// unit ideal for absPrimdecGTZ
ring R = 0, (x, y), dp;
ideal I = x + 1, y + 1, y;
LIB "primdec.lib";
def L0=absPrimdecGTZ(I);
typeof(L0);
setring L0;
primary_decomp;
absolute_primes;
kill absolute_primes, primary_decomp;
setring R;
def L1=absPrimdecGTZ(ideal(x));
typeof(L1);
setring L1;
primary_decomp;
absolute_primes;
kill absolute_primes, primary_decomp;
setring R;
def L2=absPrimdecGTZ(ideal(x*y));
typeof(L2);
setring L2;
primary_decomp;
absolute_primes;
kill absolute_primes, primary_decomp;
tst_status(1);$

