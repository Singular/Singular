LIB "tst.lib"; tst_init();

ring r;
newstruct("tt","list a, smatrix b");
matrix m[2][3]=1,2,3,4,5,6;
smatrix s=m;
tt N;
N.a=list("a","b",list("c",3));
N.b=s;
write("ssi:w ssi_newstruct",N);
def TT=read("ssi:r ssi_newstruct");
TT;
def RR=TT.r_b;
setring RR;
TT;
TT.b;
TT.a;
string(TT.b);
system("sh","/bin/rm ssi_newstruct");

tst_status(1);$
