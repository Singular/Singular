LIB "tst.lib";
tst_init();

// The modular Hilbert function can be strictly larger than the one over QQ.
// The unchecked shortcut must therefore only run on explicit request.
ring r = 0,(x,y,z),(dp(2),dp(1));
ideal I = x2+xz,x2+xz+32003y2,z2,y3;

ideal G = std(I);
G;
hilb(G);
vdim(G);

option(stdhilb);
ideal Gstdhilb = std(I);
Gstdhilb;
hilb(Gstdhilb);
vdim(Gstdhilb);
option(nostdhilb);

option(probabilistic);
ideal Gprob = std(I);
Gprob;
hilb(Gprob);
vdim(Gprob);
option(noprobabilistic);

// The modular shortcut used to miss x15 in this example. The colength check
// now detects the incomplete result and recomputes the standard basis.
ring s = 0,(x,y),ds;
ideal J = x5+32003y2,y6,(1+x)*(x5+32003y2)+y6;
ideal H = std(J);
H;
hilb(H);
vdim(H);

LIB "modular.lib";
proc markerPrimeTest(int p, alias list args) { return(1); }
proc markerDelete(alias list results) { return(list()); }
int pTestCalled;
int finalTestCalled;
proc markerPTest(string command, alias list args, alias def result, int p)
{
  pTestCalled = 1;
  return(1);
}
proc markerFinalTest(string command, alias list args, alias def result)
{
  finalTestCalled = 1;
  return(1);
}

ring m = 0,(a,b),dp;
ideal K = a2,b2;
int savedCpus = system("--cpus");
system("--cpus",1);
option(probabilistic);
ideal M = modular("std",list(K),markerPrimeTest,markerDelete,
                  markerPTest,markerFinalTest);
pTestCalled,finalTestCalled;
option(noprobabilistic);
system("--cpus",savedCpus);

tst_status(1);$
