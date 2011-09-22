LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
ideal i=xz,yz,x3-y3;
def l=res(i,0); // homogeneous ideal: uses lres
l;
print(betti(l), "betti"); // input to betti may be of type resolution
l[2];         // element access may take some time
i=i,x+1;
l=res(i,0);   // inhomogeneous ideal: uses mres
l;
ring rs=0,(x,y,z),ds;
ideal i=imap(r,i);
def l=res(i,0); // local ring not minimized: uses sres
l;
res(i,0,0);     // local ring and minimized: uses mres
tst_status(1);$
