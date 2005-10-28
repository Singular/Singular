LIB "tst.lib";
tst_init();


//======================  Remark 7.1 =============================
ring R1 = 0, x, dp;
poly f1 = 3x7+3x6-18x5-30x4+15x3+63x2+48x+12;
factorize(f1);
//->   [1]:
//->      _[1]=3
//->      _[2]=x+1
//->      _[3]=x-2
//->   [2]:
//->      1,5,2
factorize(x4-2,1);
//->   _[1]=x4-2
ring R2 = (0,a), x, dp;  minpoly = a4-2;
factorize(x4-2,1);
//->   _[1]=x2+(a2)
//->   _[2]=x+(a)
//->   _[3]=x+(-a)
ring R3 = 0, (x,y), dp;
poly f3 = x2y4+y6+2x3y2+2xy4-7x4+7x2y2+14y4+6x3+6xy2+47x2+47y2;

//fixCL: wieso haengt output von factorize( ,1) von random ab???
factorize(f3,1);
//->   _[1]=y4+2xy2-7x2+14y2+6x+47
//->   _[2]=x2+y2

if (not(defined(absFactorize))){ LIB "absfact.lib"; }
setring R1;
def R4 = absFactorize(x4-2);
setring R4;
absolute_factors;
//->   [1]:
//->      _[1]=1
//->      _[2]=x+(-a)
//->   [2]:
//->      1,1
//->   [3]:
//->      _[1]=(a)
//->      _[2]=(a4-2)
//->   [4]:
//->      4
setring R3;
def R5 = absFactorize(f3);
setring R5;
absolute_factors;
//->   [1]:
//->      _[1]=1/169
//->      _[2]=13*y2+(-14a+19)*x+(-7a+94)
//->      _[3]=x+(a)*y
//->   [2]:
//->      1,1,1
//->   [3]:
//->      _[1]=(a)
//->      _[2]=(7a2-6a-47)
//->      _[3]=(a2+1)
//->   [4]:
//->      4


kill R1,R2,R3,R4,R5;
//======================  Example 7.2 (new Session) ==================
if (not(defined(absFactorize))){ LIB "absfact.lib"; }
ring R = 0, (x,y,z), dp;
poly f = y4+z2-y2*(1-x2);
def S = absFactorize(f);
setring S;
absolute_factors[4];  // number of absolutely irreducible factors
//->   1


kill S,R;
//======================  Example 7.7 (new Session) ==================
ring R = 0, (a,b,c,d,e,f,g,h), dp;
ideal I = a+c+d-e-h, 2df+2cg+2eh-2h2-h-1, 
          3df2+3cg2-3eh2+3h3+3h2-e+4h, 
          6bdg-6eh2+6h3-3eh+6h2-e+4h, 
          4df3+4cg3+4eh3-4h4-6h3+4eh-10h2-h-1, 
          8bdfg+8eh3-8h4+4eh2-12h3+4eh-14h2-3h-1, 
          12bdg2+12eh3-12h4+12eh2-18h3+8eh-14h2-h-1, 
          -24eh3+24h4-24eh2+36h3-8eh+26h2+7h+1;
if (not(defined(primdecGTZ))){ LIB "primdec.lib"; }
int aa = timer; 
list PD = primdecGTZ(I);
size(PD);
//->   9   
ideal Prime6 = std(PD[6][2]);
dim(Prime6);
//->   2
PD[6];
//->   [1]:
//->      _[1]=h2+2h+1
//->      _[2]=gh+g           
//->              [...]
//->      _[15]=-144h5-408h4-384h3-118h2+8e+3h+9 
//->      _[16]=-144h5-408h4-384h3-118h2+8a+8c+8d-5h+9
//->   [2]:
//->      _[1]=h+1
//->      _[2]=g
//->      _[3]=f
//->      _[4]=d
//->      _[5]=-144h5-408h4-384h3-118h2+8e+3h+9
//->      _[6]=-144h5-408h4-384h3-118h2+8a+8c+8d-5h+9
ideal check = std(radical(PD[6][1]));
check;
//->   check[1]=h+1
//->   check[2]=g
//->   check[3]=f
//->   check[4]=e+1
//->   check[5]=d
//->   check[6]=a+c+2
size(reduce(Prime6,check,1));
//->   0
size(reduce(check,Prime6,1));
//->   0
ideal EI = equidimMax(I);
dim(std(EI));
//->   3
list PDEI = primdecGTZ(EI);
size(PDEI);
//->   3
aa = timer;
def S = absPrimdecGTZ(I);
setring S;
absolute_primes[6];
//->   [1]:
//->      _[1]=@c
//->      _[2]=h+1
//->           [...]
//->      _[7]=a+c+2
//->   [2]:
//->      1
absolute_primes[9];
//->   [1]:
//->      _[1]=12*@c^2+12*@c+1
//->      _[2]=h-@c
//->      _[3]=2*g+2*@c+1
//->      _[4]=f+2*@c+1
//->      _[5]=e+1
//->      _[6]=4*d+@c+1
//->      _[7]=2*c+@c+1
//->      _[8]=b+2*@c+1
//->      _[9]=4*a-7*@c+1
//->   [2]:
//->      2
int num_abs_primes;
for (int j=1; j<=size(absolute_primes); j++)
  { num_abs_primes = num_abs_primes + absolute_primes[j][2]; }
num_abs_primes;
//->   12


kill R,aa,S,num_abs_primes,j;
//======================  Remark 7.8 (new Session) ==================
if (not(defined(primdecGTZ))){ LIB "primdec.lib"; }
ring R = 0, (x,y), dp;
ideal I = xy, y2;        // ideal is mixed
list LI = equidim(I);
LI;
//->   [1]:
//->      _[1]=y
//->      _[2]=x
//->   [2]:
//->      _[1]=y
intersect(LI[1],LI[2]);
//->   _[1]=y
ideal J = xy, y3-2y2+y;
list LJ = equidim(J);
LJ;
//->   [1]:
//->      _[1]=x
//->      _[2]=y2-2y+1
//->   [2]:
//->      _[1]=y
intersect(LJ[1],LJ[2]);
//->   _[1]=xy
//->   _[2]=y3-2y2+y


kill R;
//====================  Example 7.10 (new Session) ==================
if (not(defined(normal))){ LIB "normal.lib"; }
ring R = 0, (x,y,z), dp;
ideal I = z-x4, z-y6;
list nor = normal(I);
//->   // 'normal' created a list of 1 ring(s)
def R1 = nor[1];  setring R1;  R1;
//->   //   characteristic : 0
//->   //   number of vars : 2
//->   //        block   1 : ordering a
//->   //                  : names    T(1) T(2) 
//->   //                  : weights     1    0 
//->   //        block   2 : ordering dp
//->   //                  : names    T(1) T(2) 
//->   //        block   3 : ordering C
norid;
//->   norid[1]=T(2)^2-1
normap;
//->   normap[1]=T(1)^3*T(2)^2
//->   normap[2]=T(1)^2*T(2)
//->   normap[3]=T(1)^12*T(2)^8
setring R; 
primdecGTZ(I);
//->   [1]:
//->      [1]:
//->         _[1]=y6-z
//->         _[2]=-y3+x2
//->      [2]:
//->         _[1]=y6-z
//->         _[2]=-y3+x2
//->   [2]:
//->      [1]:
//->         _[1]=y6-z
//->         _[2]=y3+x2
//->      [2]:
//->         _[1]=y6-z
//->         _[2]=y3+x2
list NOR = normal(I,1);
//->   // 'normal' created a list of 2 ring(s).
R1=NOR[1];        // the 1st ring
setring R1; R1;
//->   //   characteristic : 0
//->   //   number of vars : 1
//->   //        block   1 : ordering dp
//->   //                  : names    T(1) 
//->   //        block   2 : ordering C
norid;
//->   norid[1]=0
normap;
//->   normap[1]=T(1)^3
//->   normap[2]=-T(1)^2
//->   normap[3]=T(1)^12
def R2=NOR[2];    // the 2nd ring
setring R2; R2;
//->   //   characteristic : 0
//->   //   number of vars : 1
//->   //        block   1 : ordering dp
//->   //                  : names    T(1) 
//->   //        block   2 : ordering C
norid;
//->   norid[1]=0
normap;
//->   normap[1]=T(1)^3
//->   normap[2]=T(1)^2
//->   normap[3]=T(1)^12


kill R,nor,R1,NOR,R2;
//====================  Example 7.11 (new Session) ==================
if (not(defined(normal))){ LIB "normal.lib"; }
ring R = 0, x(1..3), dp;
poly f1 = -x(1)^3*x(2)^3*x(3)^2-x(1)^3*x(2)*x(3)^4
          -x(1)*x(2)^3*x(3)^4+x(1)^5*x(2)^2+x(1)^5*x(3)^2
          +x(1)^3*x(2)^2*x(3)^2+x(1)^2*x(2)^2*x(3)^3
          -x(1)^4*x(2)*x(3);
poly f2 = -x(1)^2*x(2)^3*x(3)^5-x(1)^2*x(2)*x(3)^7-x(2)^3*x(3)^7
          +x(1)^2*x(2)^5*x(3)^2+x(1)^4*x(2)^2*x(3)^3+x(2)^5*x(3)^4
          +x(1)^4*x(3)^5+x(1)^2*x(2)^2*x(3)^5-x(1)^2*x(2)*x(3)^6
          +x(1)*x(2)^2*x(3)^6-x(2)^3*x(3)^6-x(1)^4*x(2)^4
          -x(1)^2*x(2)^4*x(3)^2-x(1)*x(2)^4*x(3)^3+x(1)^4*x(3)^4
          -x(1)^3*x(2)*x(3)^4+x(1)^2*x(2)^2*x(3)^4
          +x(1)*x(2)^2*x(3)^5+x(1)^3*x(2)^3*x(3)
          -x(1)^3*x(2)*x(3)^3;
ideal I = f1, f2;  
list NOR = normal(I);
//->   // 'normal' created a list of 3 ring(s).
def R1 = NOR[1];      // the 1st ring
setring R1;
norid;
//->   norid[1]=T(3)2+T(4)2+T(4)T(5)-T(4)
//->   norid[2]=T(2)T(3)+T(1)T(4)+T(1)T(5)-T(1)
//->   norid[3]=T(1)T(3)-T(2)T(4)
//->   norid[4]=T(2)2+T(4)T(5)+T(5)2-T(5)
//->   norid[5]=T(1)T(2)-T(3)T(5)
//->   norid[6]=T(1)2-T(4)T(5)
normap;
//->   normap[1]=T(1)
//->   normap[2]=T(2)
//->   normap[3]=T(3)
def R2 = NOR[2];      // the 2nd ring
setring R2;  norid;
//->   norid[1]=0
normap;
//->   normap[1]=T(1)T(2)
//->   normap[2]=T(2)2
//->   normap[3]=T(1)
def R3 = NOR[3];      // the 3rd ring
setring R3;  norid;
//->   norid[1]=0
normap;
//->   normap[1]=0
//->   normap[2]=T(1)3-T(1)
//->   normap[3]=T(1)2-1
setring R; 
primdecGTZ(I);
//->   [1]:
//->      [1]:
//->         _[1]=-x(2)*x(3)^2+x(1)^2
//->      [2]:
//->         _[1]=-x(2)*x(3)^2+x(1)^2
//->   [2]:
//->      [1]:
//->         _[1]=x(1)^2*x(2)^2+x(1)^2*x(3)^2+x(2)^2*x(3)^2-x(1)*x(2)*x(3)
//->      [2]:
//->         _[1]=x(1)^2*x(2)^2+x(1)^2*x(3)^2+x(2)^2*x(3)^2-x(1)*x(2)*x(3)
//->   [3]:
//->      [1]:
//->         _[1]=-x(3)^3+x(2)^2-x(3)^2
//->         _[2]=x(1)
//->      [2]:
//->         _[1]=-x(3)^3+x(2)^2-x(3)^2
//->         _[2]=x(1)

tst_status(1);$

