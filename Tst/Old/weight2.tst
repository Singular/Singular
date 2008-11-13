option(prot);
ring h2=32003,(t,x,y,z),dp;
ideal i=
2t3x5y2z+x2t5y+2x2y,
2y3z2x+3z2t6x2y+9y2t3z,
2t5z+y3x2t+z2t3y2x5;
intvec e=weight(i);
e;
//ring r=32003,(a,b,c,d),wp(e[1],e[2],e[3],e[4]);
//map f=h2,a,b,c,d;
//ideal k=f(i);
//ideal k0=std(k);
//"//size "+string(size(k0))+
//", dim "+string(dim(k0))+", mult "+string(mult(k0))+
//", lastdeg "+string(ord(k0[size(k0)])); 
//kill r;
kill h2,e;
ring s6=32003,(x,y,z),dp;
int a =6;
int b =8;
int c =10;
int alpha =5;
int beta= 5;
int t= 0;
poly f =x^a+y^b+z^c+x^alpha*y^(beta-5)+x^(alpha-2)*y^(beta-3);
f=f+x^(alpha-3)*y^(beta-4)*z^2+x^(alpha-4)*y^(beta-4)*(y^2+t*x)^2;
ideal i= jacob(f);
i=i,f;
intvec e=weight(i);
e;
//ring r=32003,(u,v,w),wp(e[1],e[2],e[3]);
//map f=s6,u,v,w;
//ideal k=f(i);
//ideal k0=std(k);
//"//size "+string(size(k0))+
//", dim "+string(dim(k0))+", mult "+string(mult(k0))+
//", lastdeg "+string(ord(k0[size(k0)])); 
//kill r;
kill s6,e,a,b,c,alpha,beta,t;

ring s12=32003,(x,y,z,w),dp;
ideal i0=x2-z10-z20,xy3-z10-z30,y6-xy3w40;
option(redSB);
ideal i=interred(i0^3);
option(noredSB);
i;
intvec e=weight(i);
e;
//ring r=32003,(u,v,a,b),wp(e[1],e[2],e[3],e[4]);
//map f=s12,u,v,a,b;
//ideal k=f(i);
//ideal k0=std(k);
//"//size "+string(size(k0))+
//", dim "+string(dim(k0))+", mult "+string(mult(k0))+
//", lastdeg "+string(ord(k0[size(k0)])); 
//kill r;
kill s12,e;

ring pa= 32003,(k,h,g,f,e,d,c,b,a),dp;
ideal i0= af2-f5,bg2-f4g,c3-f3gh,k-fgh3,df-fh4,ef-h5;
ideal i=i0^2;
intvec E=weight(i);
E;
//ring r=32003,(u,v,w,x,y,z,l,m,n),
//wp(e[1],e[2],e[3],e[4],e[5],e[6],e[7],e[8],e[9]);
//map g=pa,u,v,w,x,y,z,l,m,n;
//ideal k0=g(i);
//ideal j=std(k0);
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)])); 
//kill r;
kill pa,E;

ring r5=32003,(a,b,c,d,e),dp;
ideal i =
   47b6+94b3c2-71b3e2+91ab3-22b4+47c4+71b3d-67b3e-71c2e2-2e4+91b3+91ac2-22bc2
+71c2d-67c2e-21ae2-80be2+4de2+82e3+53a2-23ab-47b2+91c2+21ad+80bd-2d2+55ae+18be
-82de-26e2+106a-23b+21d+55e+53,
   -98b6-196b3c2+4b3e2-54ab3-81b4-98c4-4b3d-87b3e+4c2e2-51e4-54b3-54ac2-81bc2
-4c2d-87c2e+22be2+102de2+92e3-28a2-53ab+70b2-54c2-22bd-51d2+86ae+27be-92de+73e2
-56a-53b+86e-28,
   49b6+98b3c2-49b3e2-54ab3+90b4+49c4+49b3d+57b3e-49c2e2-21e4-54b3-54ac2+90bc2
+49c2d+57c2e+18ae2-67be2+42de2-23e3+26a2-50ab+28b2-54c2-18ad+67bd-21d2-74ae+2be
+23de+58e2+52a-50b-18d-74e+26,
   34b6+68b3c2-74b3e2-30ab3-56b4+34c4+74b3d+21b3e-74c2e2+9e4-30b3-30ac2-56bc2
+74c2d+21c2e-77ae2+66be2-18de2-26e3-69a2+38ab+26b2-30c2+77ad-66bd+9d2+50ae+29be
+26de-121e2-138a+38b+77d+50e-69,
   -29b6-58b3c2+68b3e2+28ab3+89b4-29c4-68b3d-52b3e+68c2e2+13e4+28b3+28ac2+89bc2
-68c2d-52c2e-17ae2+87be2-26de2+32e3-100a2-41ab-75b2+28c2+17ad-87bd+13d2+79ae-
87be-32de+33e2-200a-41b+17d+79e-100;
intvec E=weight(i);
E;
//ring r=32003,(u,v,w,x,y),wp(e[1],e[2],e[3],e[4],e[5]);
//map g=r5,u,v,w,x,y;
//ideal k0=g(i);
//ideal j=std(k0);
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)])); 
//kill r;
kill r5,E;

ring p1=32003,(t,x,y,z,a,b,c,d,e),dp;
ideal i =
-y82a+x32z23,
x45-y13z21b,
y33z12-z41c,
-y33z12d+z22,
x5y17z22e-1,
xyzt-1;
intvec ew=weight(i);
ew;
//ring r=32003,(s,u,v,w,h,l,m,n,o),
//wp(ew[1],ew[2],ew[3],ew[4],ew[5],ew[6],ew[7],ew[8],ew[9]);
//map g=p1,s,u,v,w,h,l,m,n,o;
//ideal k0=g(i);
//ideal j=std(k0);
//size(j);
//ord(j[size(j)]);
//kill r;
kill p1,ew;

test(9);
option(weightM);
ring vc= 32003,(x,y,z,t),ds;
degBound=1;
ideal i=
49y5-9y4+41x2z+12y3+33y2+11y+73t,
33y4+21x2y+17x2z+63y3+11xy+77y2+91y+1t,
26y2+44xz+12y+9t;
ideal k=i^2;
ideal j = std(k);
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)])); 
degBound=0;
kill vc;

int N=3;
ring ra3=32003,x(1..N),ds;  
ideal i0= 
  -7*x(1)^6-49*x(1)^5*x(2)+16*x(1)^4*x(2)^2+27*x(1)^3*x(2)^3-
38*x(1)^2*x(2)^4+60*x(1)*x(2)^5+70*x(2)^6-28*x(1)^5*x(3)+85*x(1)^4*x(2)*x(3)+
69*x(1)^3*x(2)^2*x(3)-13*x(1)^2*x(2)^3*x(3)-65*x(1)*x(2)^4*x(3)+51*x(2)^5*x(3)-
70*x(1)^4*x(3)^2+37*x(1)^3*x(2)*x(3)^2+89*x(1)^2*x(2)^2*x(3)^2-
82*x(1)*x(2)^3*x(3)^2+6*x(2)^4*x(3)^2-67*x(1)^3*x(3)^3+19*x(1)^2*x(2)*x(3)^3-
12*x(1)*x(2)^2*x(3)^3-81*x(2)^3*x(3)^3+33*x(1)^2*x(3)^4-98*x(1)*x(2)*x(3)^4+
51*x(2)^2*x(3)^4+11*x(1)*x(3)^5+11*x(2)*x(3)^5-39*x(3)^6,
  5*x(1)^6-17*x(1)^5*x(2)-62*x(1)^4*x(2)^2+41*x(1)^3*x(2)^3-85*x(1)^2*x(2)^4+
57*x(1)*x(2)^5-52*x(2)^6+80*x(1)^5*x(3)+19*x(1)^4*x(2)*x(3)-
72*x(1)^3*x(2)^2*x(3)-34*x(1)^2*x(2)^3*x(3)-46*x(1)*x(2)^4*x(3)-
6*x(2)^5*x(3)-33*x(1)^4*x(3)^2+44*x(1)^3*x(2)*x(3)^2-23*x(1)^2*x(2)^2*x(3)^2-
60*x(1)*x(2)^3*x(3)^2+64*x(2)^4*x(3)^2+26*x(1)^3*x(3)^3-84*x(1)^2*x(2)*x(3)^3+
35*x(1)*x(2)^2*x(3)^3-91*x(2)^3*x(3)^3-93*x(1)^2*x(3)^4+22*x(1)*x(2)*x(3)^4+
  99*x(2)^2*x(3)^4-36*x(1)*x(3)^5+5*x(2)*x(3)^5+46*x(3)^6,
-32*x(1)^6-73*x(1)^5*x(2)-43*x(1)^4*x(2)^2-55*x(1)^3*x(2)^3-83*x(1)^2*x(2)^4-
50*x(1)*x(2)^5-74*x(2)^6-56*x(1)^5*x(3)+79*x(1)^4*x(2)*x(3)+
86*x(1)^3*x(2)^2*x(3)-27*x(1)^2*x(2)^3*x(3)+28*x(1)*x(2)^4*x(3)+71*x(2)^5*x(3)+
77*x(1)^4*x(3)^2-71*x(1)^3*x(2)*x(3)^2-63*x(1)^2*x(2)^2*x(3)^2-
22*x(1)*x(2)^3*x(3)^2+17*x(2)^4*x(3)^2+58*x(1)^3*x(3)^3+31*x(1)^2*x(2)*x(3)^3+
19*x(1)*x(2)^2*x(3)^3-36*x(2)^3*x(3)^3-15*x(1)^2*x(3)^4-9*x(1)*x(2)*x(3)^4+
42*x(2)^2*x(3)^4-61*x(1)*x(3)^5-67*x(2)*x(3)^5+31*x(3)^6; 
ring r=32003,(a,b,c),ds;
map ff=ra3,a+b2,b2-c3,c;
ideal i=ff(i0);
ideal j=std(i);
j;
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)]));
kill ra3,r,N;

ring r1=32003,(t,x,y,z),ds;
ideal i=
5t3x2z+2t2y3x5,
7y+4x2y+y2x+2zt,
3tz+3yz2+2yz4;
ideal j=std(i);
j;
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)]));
kill r1;
 
ring r2=32003,(x,y,z),ds;
poly p1 = 1x3y2+21328x5y+10667x2y4+21328x2yz3+10666xy6+10667y9;
poly p2 = 1x2y2z2+3z8;
poly p3 = 5x4y2+4xy5+2x2y2z3+1y7+11x10;
ideal i=p1,p2,p3;
ideal j=std(i);
j;
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)]));
kill r2;

ring r12=32003,(x,y,z,w),ds;
ideal i=x2-z10-z20,xy3-z10-z30,y6-xy3w40;
ideal j=std(i);
j;
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)]));
kill r12;

ring r17=32003,(x,y,z),ds;
ideal i=
1x3+1y4+2xz3+1z5+32000x4y2+2z6+3z7,
1xz3+32001x4y2+1z6+2z7,
9x3z2+18x2z5+31998z7+12x4y2z2+42x2z6+40x3y2z4+7z9+24x3y2z5,
31999y3z3+31991x6y+32x3y5+32001x4yz3,
12xy3z2+6x5yz2+24y3z5+20x4yz4+56y3z6+12x4yz5;
ideal j=std(i);
j;
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)]));
kill r17;

ring r18=32003,(t,x,y,z),ds;
ideal i=
4t2z+6z3t+3z3+tz,
5t2z7y3x+5x2z4t3y+3t7,
6zt2y+2x8+6z2y2t+2y5;
ideal j=std(i);
j;
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)]));
kill r18;
 
ring r5=32003,(x,y,z),ds;
int a=7;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal i= jacob(f);
test(-9);
ideal j=std(i);
j;
//"//size "+string(size(j))+
//", dim "+string(dim(j))+", mult "+string(mult(j))+
//", lastdeg "+string(ord(j[size(j)]));
kill r5;
LIB "tst.lib";tst_status(1);$
