LIB "tst.lib";
tst_init();

LIB "surfacesignature.lib";

example brieskornSign;

ring R = 0, (x,y), dp;
poly f, g, g1, g2, g3;
int N;
number a,b,c;

N = 5;
f = x15-21x14+8x13y-6x13-16x12y+20x11y2-x12+8x11y-36x10y2    
   +24x9y3+4x9y2-16x8y3+26x7y4-6x6y4+8x5y5+4x3y6-y8;
a = signature(N,f,1);
b = signature(N,f,2);
a;
a == b;

g1 = f^3 + x9y8;
signature(N,g1,2);

g3 = f^5 + x20y20;
signature(N,g3,2);


N = 6;
f = y4+2x3y2+x6+x5y;
a = signature(N,f,1);
b = signature(N,f,2);
c = signature(N,f,3);
a;
a == b;
b == c;

g1 = f^2 + x5y5;
a = signature(N,g1,1);
b = signature(N,g1,2);
a;
a == b;

g2 = f^3 + x11y11;
a = signature(N,g2,1);
b = signature(N,g2,2);
a;
a == b;


N = 7;
f = x5+y11;
a = signature(N,f,1);
b = signature(N,f,2);
c = signature(N,f,3);
a;
a == b;
b == c;

g1 = f^2 + x5y5;
b = signature(N,g1,2);
c = signature(N,g1,3);
b;
b == c;

g2 = f^3 + x11y11;
a = signature(N,g2,1);
b = signature(N,g2,2);
a;
a == b;


N = 6;     
f = x71+6x65+15x59-630x52y6+20x53+6230x46y6+910x39y12+15x47
    -7530x40y6+14955x33y12-285x26y18+6x41+1230x34y6+4680x27y12
    +1830x20y18+30x13y24+x35-5x28y6+10x21y12-10x14y18+5x7y24-y30;
a = signature(N,f,1);
b = signature(N,f,2);
a;
a == b;


N = 12;
f = 2x3y3-2y5+x4-xy2;
b = signature(N,f,2);
c = signature(N,f,3);
b;
b == c;

f = -x3y3+x6y+xy6-x4y4;
b = signature(N,f,2);
c = signature(N,f,3);
b;
b == c;    

tst_status(1);$