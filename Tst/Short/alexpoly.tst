// File: alexpoly.tst
// Tests for alexpoly lib
LIB "tst.lib";
tst_init();
LIB "alexpoly.lib";
// ------------ test of resolutiongraph: -------------------
ring r=0,(x,y),ds;
poly f1=x2-y2;
poly f2=x2+y+y2;
poly f3=(x2+y3)*(x2+y3+xy2);
resolutiongraph(f1);
resolutiongraph(f2);
resolutiongraph(f3);
resolutiongraph(2x2+3xy+4xy3-x2y);
resolutiongraph(x3-y5);
poly p_1 = y2+x3;
poly p_2 = p_1^2 + x5y;
poly p_3 = p_2^2 + x^10 *p_1;
poly p_4 = p_3^2 + x^20 *p_2;
poly p_5 = p_4^2 + x^40 *p_3;
resolutiongraph(p_1*p_2*p_3);
resolutiongraph(p_5*p_3);
resolutiongraph((x7-y3)*(y4-2x3y2-4x5y+x6-x7)*(x2-y11));
poly heme=xy8+y8+x4y6+4x3y6+2x5y5+6x6y4+4x8y3+x10y2+4x9y2+2x11y+x12;
list hne=develop(heme);
resolutiongraph(hne);
resolutiongraph(-x23-2x22-x21-42x19y2+7x18y2-70x16y4-21x15y4-14x13y6+35x12y6-35x9y8+21x6y10-7x3y12+y14);
resolutiongraph(-x29-x28+7x24y-21x20y2+35x16y3-35x12y4+21x8y5-7x4y6+y7);
intvec v=6,9,16;
resolutiongraph(v);
intmat M1[2][2]=0,3,3,0;
intvec v1=2,3;
intvec v2=4,6,7;
list vectors=v1,v2;
list L1=M1,vectors;
resolutiongraph(L1);
intmat M2[3][3]=0,2,4,2,0,2,4,2,0;
intvec u1=6,8,9;
intvec u2=6,14,15;
intvec u3=8,10,11;
resolutiongraph(list(M2,list(u1,u2,u3)));
// ------------ test of totalmultiplicities: -------------------
totalmultiplicities(x6-y4);
totalmultiplicities(hnexpansion((y-x2+x3)*(y-x2-x3)));
totalmultiplicities((x7-2x4y2+xy4-1y5)*(x7-4x4y2+4xy4-1y5));
totalmultiplicities((y2-x3)*(y2-x3-x4));
totalmultiplicities((y2-x3-x4)*(y2+x3+x4));
totalmultiplicities(((x2-y)^2+x5)*((2x2-y)^2+x5));
totalmultiplicities((x2-y4)*(x+y4));
poly g1=-x9+x8-6x7y+3x6y2-2x4y3-3x3y4+y6;
poly g2=-x21+x20-8x18y-4x15y2-8x13y3+6x10y4-4x5y6+y8;
totalmultiplicities(g1*g2);
poly k1=-x19+x18-12x17y-6x15y2-40x14y3+15x12y4-12x11y5-20x9y6+15x6y8-6x3y10+y12;
poly k2=x22-x21-14x20y+7x18y2-70x17y3-21x15y4-42x14y5+35x12y6-2x11y7
-35x9y8+21x6y10-7x3y12+y14;
poly k3=-x17-2x16-x15-20x13y2+5x12y2-10x10y4-10x9y4+10x6y6-5x3y8+y10;
totalmultiplicities(k1*k2*k3);
totalmultiplicities((x2-y3)*(x3-y5)*(x5-y7)*(x7-y11)*(x11-y13));
totalmultiplicities((x3+3x2y-xy4+y10)*(x3-x2y+y8));
intmat m1[2][2]=0,10,10,0;
intvec v3=9,10;
intvec v4=10,11;
list L2=m1,list(v3,v4);
totalmultiplicities(L2);
intmat M3[3][3]=0,4,5,4,0,4,5,4,0;
intvec z1=21,28,36;
intvec z2=25,30,33;
intvec z3=24,32,35;
totalmultiplicities(list(M2,list(z1,z2,z3)));
// ------------ test of alexanderpolynomial: -------------------
poly h1=-x11+x10-4x8y-2x5y2+y4;
poly h2=x7-y8;
poly h3=x15-y16;
list ALEX=alexanderpolynomial(h1*h2);
def ALEXring=ALEX[1];
setring ALEXring;
alexpoly;
zeta_monodromy;
alexnumerator;
alexdenominator;
setring r;
kill ALEXring;
ALEX=alexanderpolynomial(h1*h2*h3);
def ALEXring=ALEX[1];
setring ALEXring;
alexpoly;
zeta_monodromy;
alexnumerator;
alexdenominator;
setring r;
kill ALEXring;
intvec vvv=18,27,30,31;
ALEX=alexanderpolynomial(vvv);
def ALEXring=ALEX[1];
setring ALEXring;
alexpoly;
zeta_monodromy;
alexnumerator;
alexdenominator;
setring r;
kill ALEXring;
poly ff1=-x27-x25-15x24y-30x23y2+5x20y3-135x19y4+3x18y5-10x15y6-90x14y7
+10x10y9-3x9y10-5x5y12+y15;
poly ff2=x5-y11;
ALEX=alexanderpolynomial(hnexpansion(ff1*ff2));
def ALEXring=ALEX[1];
setring ALEXring;
alexpoly;
zeta_monodromy;
alexnumerator;
alexdenominator;
setring r;
kill ALEXring;
// ------------ test of charexp2multseq:  ------------------
intvec vv=8,20,30,31;
charexp2multseq(vv);
intvec vv1=18,27,75,125;
charexp2multseq(vv1);
intvec vv2=27,36,60,100;
charexp2multseq(vv2);
intvec vv3=2,3;
charexp2multseq(vv3);
intvec vv4=3,7;
charexp2multseq(vv4);
intvec vv5=4,6,7;
charexp2multseq(vv5);
intvec vv6=5,8;
charexp2multseq(vv6);
intvec vv7=6,15,19;;
charexp2multseq(vv7);
intvec vv8=7,16;
charexp2multseq(vv8);
intvec vv9=8,12,30,34;
charexp2multseq(vv9);
intvec vv10=9,21,23;
charexp2multseq(vv10);
intvec vv11=10,35,41;;
charexp2multseq(vv11);
intvec vv12=30,115,1001;
charexp2multseq(vv12);
intvec vv13=100,150,375,420,672;
charexp2multseq(vv13);
// ------------ test of multseq2charexp: ------------------------
intvec w1=2,1,1;
intvec w2=3,3,1,1,1;
intvec w3=4,2,2,1,1;
intvec w4=5,3,2,1,1 ;
intvec w5=6,6,3,3,3,1,1,1 ;
intvec w6=7,7,2,2,2,1,1;
intvec w7=8,4,4,4,4,4,4,2,2,2,2 ;
intvec w8=9,9,3,3,3,2,1,1 ;
intvec w9=10,10,10,5,5,5,1,1,1,1,1 ;
intvec w10=30,30,30,25,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,1,1,1,1;
intvec w11=100,50,50,50,50,50,50,25,25,25,20,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,2,2,1,1 ;
intvec w12=8,8,4,4,4,4,2,2,1,1;
intvec w13=18,9,9,9,9,9,9,9,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,1,1;
intvec w14=27,9,9,9,9,9,6,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,1;
intvec w15=36,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,6,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1;
intvec w16=21,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,1,1;
list L3=w1,w2,w3,w4,w5,w6,w7,w8,w9,w10,w11,w12,w13,w14,w15,w16;
for (int i=1;i<=16;i++)
{
  multseq2charexp(L3[i]);
}
// ------------ test of charexp2poly: -------------------
intvec a1=30,45,50,53;
vector b1=[1,1,1];
charexp2poly(a1,b1);
intvec a2=24,40,60,180,181;
vector b2=[1,1,1,1];
charexp2poly(a2,b2);
intvec a3=80,120,300,301;
vector b3=[1,1,1];
charexp2poly(a3,b3);
// ------------ test of tau_es --------------------------
for (i=2;i<=20;i++)
{
  tau_es(x^2-y^i);
}
for (i=2;i<=10;i++)
{
  tau_es(hnexpansion(y*(x^2-y^i)));
}
tau_es((x-y)*(x-2y)*(x-3y)*(x-4y));
tau_es((x-y)*(x-2y)*(x-3y)*(x-4y)*(x-5y));
tau_es(a1);
tau_es(L2);
// --------------- additions: -----------------------------
example totalmultiplicities;
example alexanderpolynomial;
example charexp2multseq;
example multseq2charexp;
example charexp2poly;
example tau_es;
// --------------------------------------------------------
tst_status(1);$

