LIB "tst.lib";
tst_init();
LIB "spcurve.lib";
LIB "stratify.lib";
example prepMat;
intvec watchProgress=0,1,1,1,1;
example stratify;

ring r=0,(x,y,z),ds;
matrix M2[3][2]=z,0,y^2,z-x^7,x^9,y;
def KS=KSpencerKernel(M2);
ring rt=0,(T(1..8)),wp(8,5,2,10,7,4,1,2);
def KS=imap(reneu,KS);
intvec wr=10,8,7,5,4,2,2,1;
intvec ws=9,6,3,0;
int step=3;
list l=stratify(KS,wr,ws,step);
l;

ring r=0,(x,y,z),ds;
matrix M1[3][2]=z,x^9,x,y,y,z;
def KS=KSpencerKernel(M1);
ring rt=0,(T(1..2)),wp(5,2);
def KS=imap(reneu,KS);
intvec wr=5,2;
intvec ws=3,0;
int step=3;
list l=stratify(KS,wr,ws,step);
l;

tst_status(1);$
