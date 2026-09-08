LIB "tst.lib";
tst_init();

"polynomial algorithms";
ring r=32003,(x,y),dp;
ideal G=x2+y,x+y2;
G=std(G);
attrib(G,"isSB",1);
poly c=fglmCombination(G,x2+xy+y2+x+y+1);
c==y2+x;
reduce(c,G)==0;

matrix A[2][2]=1,1,0,1;
matrixMinpoly(A)==(x-1)^2;
fastMult(x20+x+1,x15+x2+2,"univariate")
  ==(x20+x+1)*(x15+x2+2);
powerWithAlgorithm(x+y+1,5,"fast")==((x+y+1)^5);
powerWithAlgorithm(x+y+1,5,"standard")==((x+y+1)^5);

ring rq=0,(x,y),dp;
powerWithAlgorithm(x+y+1,5,"multinomial")==((x+y+1)^5);
bitSubst(x3+x+1,x+y)==6xy+2x+2y+1;
evaluateAt(x3+xy+1,2,3)==15;

list qr=divrem((x+1)*(x+y)+1,x+1);
((x+1)*qr[1]+qr[2])==((x+1)*(x+y)+1);

"ideal division";
ideal I=x3+y3+xy,x2y+y;
ideal J=x,y2;
attrib(J,"isSB",1);
list d=idealDivRem(I,J);
matrix(I)==matrix(J)*d[2]+matrix(d[1]);
list u=idealDivRem(I,J,"withUnit");
matrix(I)*matrix(u[3])==matrix(J)*matrix(u[2])+matrix(u[1]);

"coefficient extraction";
poly f=x2+3xy+2;
vector v=[x2+2xy,xy+3];
ideal K=f,xy+1;
module M=[x2+2xy,xy+3],[xy+1,x];
coeffTerm(f,xy)==3;
coeffTerm(v,[xy,0])==2;
ideal ci=coeffTerm(K,xy);
ci[1]==3 and ci[2]==1;
module cmv=coeffTerm(M,[xy,0]);
string(cmv[1])=="2" and string(cmv[2])=="1";
coeffTerm(v,xy)==[2,1];
module cmp=coeffTerm(M,xy);
cmp[1]==[2,1] and cmp[2]==[1,0];
coeffTerm(v,ideal(xy,x2))==[2,1,1];
coeffTerm(v,module([xy,0],[0,xy]))==[2,0,1];

"integer and ring helpers";
intvecMatchingSegments(intvec(1,2,1,2,1),intvec(1,2))==intvec(1,3);
intvecOverlap(intvec(1,2,3,4),intvec(3,4,5))==2;
bigint z=loadBigint("experimental_interfaces.bigint");
z==123456789012345678901234567890;
def sr=subring(x);
setring sr;
nvars(basering)==1;
setring rq;
kill sr;

"conditional interfaces";
int hnf_ok=1;
if (defined(HNF))
{
  intmat H[2][2]=2,4,6,8;
  intmat HH=HNF(H);
  hnf_ok=HH[1,1]==2 and HH[1,2]==0 and HH[2,1]==0 and HH[2,2]==4;
}
hnf_ok;

int ccluster_ok=1;
if (defined(ccluster))
{
  number zero=0;
  number width=4;
  number epsilon=1/1000;
  list clusters=ccluster(x2-1,zero,zero,width,epsilon);
  ccluster_ok=(size(clusters)==2);
}
ccluster_ok;

ring ri=3,(x,y),dp;
int irred=probIrredTest(x2+xy+y2+1,"0.1");
irred>=-1 and irred<=1;

"system interfaces";
ring rs=0,(u,v),(s,dp);
system("setsyzcomp",2);
list rs_data=ringlist(rs);
string(rs_data[3][1][1])=="s" and rs_data[3][1][2]==2;
system("gcd","EZGCD",0);
system("gcd","EZGCD",1);
system("cache_chinrem",1);
system("cache_chinrem",0);
system("NCGetType")==-1;
system("mults")>=0;
int nc_extensions=system("NCUseExtensions");
system("NCUseExtensions",nc_extensions)==nc_extensions;

ring ncbase=0,(a,b),dp;
def Wpair=nc_algebra(1,1);
setring Wpair;
b*a==a*b+1;
system("ForceNewNCMultiplication");
b*a==a*b+1;

setring ncbase;
def Wpower=nc_algebra(1,1);
setring Wpower;
b*a==a*b+1;
system("ForceNewOldNCMultiplication");
b*a==a*b+1;

setring ncbase;
def exterior=nc_algebra(-1,0);
setring exterior;
system("ForceSCA",1,2);
a2==0 and b2==0 and b*a==-a*b;

"library adapters";
ring rl=0,(a,b),ds;
vector lv=[a2+b,ab+1];
module lm=[a,0],[0,b];
attrib(lm,"isSB",1);
list ln=locNF(lv,lm,10,intvec(1,1));
ln[1][1,1]==b and ln[1][2,1]==1;
ln[2][1,1]==a and ln[2][2,1]==a;

setring rq;
ideal Q=x2,xy,y2;
resolution R=mres(Q,0);
resolution MR;
module T;
MR,T=minresWithMap(R);
typeof(MR)=="resolution" and typeof(T)=="module";

LIB "elim.lib";
ideal S=x2,xy;
ideal P=x;
ideal SP=sat(S,P,"principal");
ideal SG=sat(S,P);
reduce(SP,std(SG))[1]==0 and reduce(SG,std(SP))[1]==0;

LIB "ssi.lib";
string serialized=writeToString(Q);
def restored=readFromString(serialized);
typeof(restored)=="ideal";
reduce(Q,std(restored))[1]==0 and reduce(restored,std(Q))[1]==0;

tst_status(1); $
