// standard examples for homogeneous ideals

//==== elem 30, dim 0, mult 131;
//==== Macaulay 1 sec
//=== 0.54 sec

ring b1=32003,(a,b,c,d,e),dp;
int n =10;
poly s1=a+b+c+d+e;
poly s2=de+1cd+1bc+1ae+1ab;
poly s3=cde+1bcd+1ade+1abe+1abc;
poly s4=bcde+1acde+1abde+1abce+1abcd;
poly s5=abcde;
ideal i=s1,s2,s3,s4,s5,a^n,b^n,c^n,d^n,e^n;
ideal i0=std( i);
size(i0);
degree (i0);
kill n ,b1;

//==== 9 9 9 homog
//==== elem 83, dim 1, mult 512;
//=== Macaulay 0 sec
//=== 0.24 sec
ring s2=32003,(t,x,y,z),dp;
ideal i=
9x8+y7t+5x4y2t2+2xy2z3t2,
9y8+7xy6t+2x5yt2+2x2yz3t2,
9z8+3x2y2z2t2;
ideal i0=std( i);
size(i0);
degree (i0);
kill s2;

//========= Beipiel Alex1 homog
//===== elem 95, dim 3, mult 18
//==== Macaulay 5  sec
//=== 3.39 sec

ring h1=32003,(w,x,t,y,z),dp;
ideal j=
9x8+y7t3z4+5x4y2t2+2xy2z3t2,
9y8+7xy6t+2x5y4t2+2x2yz3t2,
9z8+3x2y3z2t4;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill h1;

//========= Beipiel Alex2 homog
//=== elem 158, dim 3, mult 52
//==== Macaulay 5 min 0 sec
//=== 122.43 sec

ring h2=32003,(t,x,y,z,w),dp;
ideal j=
2t3x5y2z+x2t5y+2x2y,
2y3z2x+3z2t6x2y+9y2t3z,
2t5z+y3x2t+z2t3y2x5;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill h2;

//========= Beipiel Alex3 homog
//=== elem 211, dim 3 , mult 7
//==== Macaulay 27 sec
//=== 19.43 sec

ring h3=32003,(t,x,y,z,w),dp;
ideal j=
5t3x8y2z+x2t5y+2x2y+5t,
7y6z2x+6x2y+9y2t3z+4t6x,
4t6z+8y3x5t+2t3y2x3+3x;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill h3;

//========= Beipiel Alex4 homog
//=== elem 116, dim 3, mult 87
//==== Macaulay 5 sec
//=== 4.72 sec

//========= Beipiel Alex5 homog
//=== elem 52, dim 3, mult 8
//=== Macaulay 17 sec
//=== 6.99 sec

ring h5=32003,(t,x,y,z,w),dp;
ideal j=
4t2z+6z3t+3z2,
5t2z7y3x+5x2z4t3y+3t,
6zt2y+2x+6z2y2t+2y;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill h5;

//========= Beipiel Alex6 homog
//=== elem 49, dim 3, mult 5
//=== Macaulay 27 sec
//=== 18.14 sec

ring h6=32003,(t,x,y,z,w),dp;
ideal j=
4t2z+6z3t+3z2tx+3z,
5t7yx+5x2z4t3y+3t,
6zt2y+2x+6z2y2t+2y;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill h6;

//==== Beispiel Behnke
//==== elem 149, dim 1, mult 120
//==== Macaulay 5 sec
//=== 3.82 sec

ring be=32003,(a,b,c,d,e),dp;
ideal i=
a5-b5,
b5-c5,
c5-d5,
d5-e5,
a4b+b4c+c4d+d4e+e4a;
test(15);
ideal i0=std( i);
size(i0);
degree (i0);
test(-15);
kill be;

//==== elem 82, dim 3, mult 3
//==== Macaulay 1 min 11 sec
//=== 17.50 sec

ring rh1=32003,(w,t,x,y,z),dp;
ideal j=
5t3x2z+2t2y3x5,
7y+4x2y+y2x+2zt,
3tz+3yz2+2yz4;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh1;

//==== elem 68, dim 1, mutl 720
//==== Macaulay 3 sec
//=== 1.27 sec

ring rh2=32003,(w,x,y,z),dp;
poly p1 = 1x3y2+21328x5y+10667x2y4+21328x2yz3+10666xy6+10667y9;
poly p2 = 1x2y2z2+3z8;
poly p3 = 5x4y2+4xy5+2x2y2z3+1y7+11x10;
ideal j= p1,p2,p3;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh2;

//==== elem 75, dim 1, mult 720
//==== Macaulay 3 sec
//=== 1.53 sec

ring rh3=32003,(w,x,y,z),dp;
int a =11;
int b =10;
int c =3;
int t =1;

poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j= jacob(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh3;

//=== elem 41, dim 1, mult 125
//==== Macaulay 4 sec
//=== 1.60 sec

ring rh4=32003,(w,x,y,z),dp;
int a=6;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal j= jacob(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh4;

//=== elem 44, dim 1, mult 216
//==== Macaulay 4 sec
//=== 1.69 sec

ring rh5=32003,(w,x,y,z),dp;
int a =7;
poly f=xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal j= jacob(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh5;

//=== elem 117, dim 1, mult 107
//==== Macaulay 3 sec
//=== 2.85 sec

ring rh6=32003,(w,x,y,z),dp;
int a= 6;
int b= 8;
int c =10;
int alpha=5;
int beta= 5;
int t=0;
poly f= x^a+y^b+z^c+x^alpha*y^(beta-5)+x^(alpha-2)*y^(beta-3)+x^(alpha-3)*y^(beta-4)*z^2+x^(alpha-4)*y^(beta-4)*(y^2+t*x)^2;
ideal j= jacob(f);
j=j,(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill a, b, c, alpha ,beta, t, rh6;

//=== elem 66, dim 1, mult 315
//==== Macaulay 4 sec
//=== 1.79 sec

ring rh7=32003,(u,x,y,z),dp;
int a= 6;
int b= 8;
int c =10;
int alpha=5;
int beta= 5;
int t=0;
poly f =x^a+y^b+z^c+x^alpha*y^(beta-5)+x^(alpha-2)*y^(beta-3)+x^(alpha-3)*y^(beta-4)*z^2+x^(alpha-4)*y^(beta-4)*(y^2+t*x)^2;
ideal j =jacob(f);
ideal i=homog(j,u);
ideal i0=std( i);
size(i0);
degree (i0);
kill a, b, c, alpha ,beta, t, rh7;

//=== elem 169, dim 1, mult 251
//==== Macaulay 6 sec
//=== 3.65

ring rh9=32003,(w,x,y,z),dp;
int a =11;
int b =10;
int c =3;
int t =1;

poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j= jacob(f);
j=j,(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill b,c,t,a,rh9;

//=== elem 75, dim 1, mult 44
//==== Macaulay 2 sec
//=== 1.71 sec

ring rh10=32003,(w,x,y,z),dp;
int a =6;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal j= jacob(f);
j=j,(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);

kill a,rh10;

//=== elem 61, dim 1, mult 67
//==== Macaulay 1 sec
//=== 0.78 sec

ring rh11=32003,(w,x,y,z),dp;
int a =7;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal j= jacob(f);
j=j,(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill a,rh11;

//=== elem 83, dim 2, mult 26400
//==== Macaulay 1 sec
//=== 0.46 sec

ring rh12=32003,(t,x,y,z,w),dp;
ideal j=x2-z10-z20,xy3-z10-z30,y6-xy3w40;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh12;

//=== elem 23, dim 1, mult 380
//==== Macaulay 4 sec
//=== 1.81 sec

ring rh13=32003,(w,x,y),dp;
poly f=(x+y)^20 +y21+(x+y)^10 +x9y9+x3y15+x2y11+x4y4+(x+y)^17;
ideal j= jacob(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh13;

//=== elem 254, dim 1, mult 56
//==== Macaulay 44 sec
//=== 23.08 sec

ring rh14=32003,(w,x,y),dp;
poly f=(x+y)^20 +y21+(x+y)^10 +x9y9+x3y15+x2y11+x4y4+(x+y)^17;
ideal j= jacob(f);
j=j,(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh14;

//=== elem 143, dim 1, mult 194
//==== Macaulay 11 sec
//===  2.05 sec

ring rh17=32003,(w,x,y,z),dp;
ideal j=
1x3+1y4+2xz3+1z5+32000x4y2+2z6+3z7,
1xz3+32001x4y2+1z6+2z7,
9x3z2+18x2z5+31998z7+12x4y2z2+42x2z6+40x3y2z4+7z9+24x3y2z5,
31999y3z3+31991x6y+32x3y5+32001x4yz3,
12xy3z2+6x5yz2+24y3z5+20x4yz4+56y3z6+12x4yz5;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh17;

//=== elem 22, dim 2, mult 416
//==== Macaulay 4 sec
//===  0.77 sec

ring rh18=32003,(w,t,x,y,z),dp;
ideal j=
4t2z+6z3t+3z3+tz,
5t2z7y3x+5x2z4t3y+3t7,
6zt2y+2x8+6z2y2t+2y5;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh18;

//===== elem 111,dim 1, mult 3564
//==== Macaulay 15 sec
//===  9.11 sec

ring rh19=32003,(w,x,y,z),dp;
int a =19;
int b =19;
int c =4;
int t =1;

poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j= jacob(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill b,c,t,a,rh19;

//==== elem 278,dim 1, mult 8602
//==== Macaulay 6 min 9 sec
//===  239.13 sec (3 min 59.13 sec)

ring rh20=32003,(w,x,y,z),dp;
int a =24;
int b =23;
int c =6;
int t =1;

poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j= jacob(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill b,c,t,a,rh20;

//=== elem 103, dim 1, mult 729
//=== Macaulay 4 min 33 sec
//=== 144.50 sec (2 min 24.50 sec)

ring testh1=32003,(w,x,y,z),dp;
int a =10;
poly f=(xyz+xy+yz+xz)*(x+y+z)^2 +x^a+y^a+z^a;
ideal j= jacob(f);
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill a,testh1;

//==== elem 443, dim 2, mult 392
//==== Macaulay 29 min 17 sec
//===  1019.78 sec (16 min 59.78 sec)

ring testh2=32003,(a,b,c,d,e,f,g,h),dp;
ideal i=
a+b+c+d+e+f+g,
ab+bc+cd+de+ef+fg+ga,
abc+bcd+cde+d*ef+efg+fga+gab,
abcd+bcde+cdef+defg+efga+fgab+gabc,
abcde+bcdef+cdefg+defga+efgab+fgabc+gabcd,
abcdef+bcdefg+cdefga+defgab+efgabc+fgabcd+gabcde,
abcdefg+h^7;
ideal i0=std(i);
size(i0);
degree (i0);
kill testh2;

//===  elem 767, dim 2 mult 86
//== Macaulay 42 min 0 sec
//=== 1616.59 sec (26 min 56.59 sec)

ring rh15=32003,(w,x,y,z),dp;
poly p1= x2y *(47x5y7z3+28xy5z8+63+91x5y3z7);
poly p2= xyz *(57y6+21x2yz9+51y2z2+15x2z4);
poly p3= xy4z *(74y+32x6z7+53x5y2z+17x2y3z);
poly p4= y3z *(21x2z6+32x10y6z5+23x5y5z7+27y2);
poly p5= xz *(36y2z2+81x9y10+19x2y5z4+79x4z6);
ideal j= p1,p2,p3,p4,p5;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
kill rh15;

//=== elem  ,dim 0, mult 90
//=== Macaulay 3 sec
//===  2.21

ring testh4 = 31991,(u,v,w,x,y,z),dp;

ideal i = xy+y2+uz-wz-xz+yz, x2-y2+uz-wz-xz+z2, wy, wx-uz+yz, w2-y2+uz-z2, vz-yz-z2,
vy-y2-wz+xz+yz+z2, vx-y2+uz+yz-z2, vw-y2+uz+yz-z2, v2+y2+uz-xz-yz-z2, uy+y2+yz,
ux-y2+wz-xz-z2, uw-y2+uz+yz-z2, uv-y2-xz+yz, u2+yz;

ideal i3 = i^2;
ideal j = std(i3);
size(j);
degree(j);

kill testh4;
$
