LIB "tst.lib";
tst_init();

ring S=0,(x,y,z,u,v),dp;
ideal I;
I[1]=6z-1;
I[2]=6y-1;
I[3]=12x+6u+6v+1;
I[4]=12u2+12v2-1;
I[5]=432v4-36v2+1;
dim(std(I));
//option(prot);
def J=quot(module(I),module(ideal(xyzuv)),1);
dim(std(J));kill J;
def J=quot(module(I),module(ideal(xyzuv)),2);
dim(std(J));kill J;
def J=quot(module(I),module(ideal(xyzuv)),3);
dim(std(J));kill J;
def J=quot(module(I),module(ideal(xyzuv)),4);
dim(std(J));
J;kill J;
def J=quotient(I,ideal(xyzuv));
dim(std(J));
J;kill S;
tst_status(1);$
