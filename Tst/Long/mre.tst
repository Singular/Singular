LIB "tst.lib";
tst_init();
option(prot);
ring r;
ideal i=xy+yz+xz,x3,y3,z3;
list I=mres(i,0);
I;
kill r;
ring an=32003,(w,x,y,z),(dp,C);
ideal i=
1w2xy+1w2xz+1w2yz+1wxyz+1x2yz+1xy2z+1xyz2,
1w4x+1w4z+1w3yz+1w2xyz+1wx2yz+1x2y2z+1xy2z2,
1w6+1w5z+1w4xz+1w3xyz+1w2xy2z+1wx2y2z+1x2y2z2;
list I=mres(i,0);
I;
kill an;
ring an=32003,(w,x,y,z),(dp,c);
ideal i=
wx2+y3,
xy2+z3,
yz2+w3,
zw2+x3,
xyz+yzw+zwx+wxy;
list I=mres(i,0);
I;
kill an;
ring an=32003,(w,x,y,z),(dp,c);
ideal i=
wx+y2,
xy+z2,
yz+w2,
zw+x2,
xy+yz+zw+wx;
list I=mres(i,0);
I;
kill an;
ring an=32003,(x,y,z),(dp,c);
ideal i=
zx2+y3,
xy2+z3,
yz2+x3,
x2y+y2z+z2x;
list I=mres(i,0);
I;
kill an;
ring cyc5 = 32003,(f,a,b,c,d,e),dp;
ideal i=
a+b+c+d+e,
de+1cd+1bc+1ae+1ab,
cde+1bcd+1ade+1abe+1abc,
bcde+1acde+1abde+1abce+1abcd,
abcde-f5;
//std(i);
list I=mres(i,0);
I;
kill cyc5;
ring r=32003,(y,t,x,z,w),dp;
ideal i=6*t^5*x^3*z^4+5*x^5*z^7+4*t^4*y*z^3*w^4,
5*t^3*x^2*y^7*z^4+5*t^6*x*y^3*z^3*w^3+4*t^5*x^5*y*w^5,
2*t^8*z^6+6*t^5*y*z^7*w+6*t^4*y^2*z^3*w^5;
//i;
//option(prot);
//ideal j=std(i);
//$
list I=mres(i,0);
I;
kill r;
ring cyc5 =32003,(a,b,c,d,e,f),dp;
ideal i=
a5-b5,
b5-c5,
c5-d5,
d5-e5,
a4*b+b4*c+c4*d+d4*e+a*e4;
list I=mres(i,0);
I;
kill cyc5;
ring an=32003,(t,x,y,z,w),(dp,C);
ideal i=
2t3x5y2z+t5x2yw3+2x2yw8,
3t6x2yz2+9t3y2zw5+2xy3z2w5,
t3x5y2z2+tx2y3w6+2t5zw6;
list I=mres(i,0);
I;
kill an;
ring r=0,(x,y,z,t,w),dp;
ideal i=6*t^5*x^3*z+5*x^5*z^4+4*t^4*y*w^4,
5*x*y^6*z^4+5*t^3*y^2*z^3*w^3+4*t^2*x^4*w^5,
2*t^4*z^3+6*t*y*z^4*w+6*y^2*w^5;
list I=mres(i,0);
I;
kill r;
ring r=32003,(x,y,z),(C,dp);
ideal i=xy+yz+xz,x3,y3,z3;
list I=mres(i,0);
I;
kill r;
ring an=32003,(w,x,y,z),(C,dp);
ideal i=
1w2xy+1w2xz+1w2yz+1wxyz+1x2yz+1xy2z+1xyz2,
1w4x+1w4z+1w3yz+1w2xyz+1wx2yz+1x2y2z+1xy2z2,
1w6+1w5z+1w4xz+1w3xyz+1w2xy2z+1wx2y2z+1x2y2z2;
list I=mres(i,0);
I;
kill an;
ring an=32003,(w,x,y,z),(C,dp);
ideal i=
wx2+y3,
xy2+z3,
yz2+w3,
zw2+x3,
xyz+yzw+zwx+wxy;
list I=mres(i,0);
I;
kill an;
ring an=32003,(w,x,y,z),(C,dp);
ideal i=
wx+y2,
xy+z2,
yz+w2,
zw+x2,
xy+yz+zw+wx;
list I=mres(i,0);
I;
kill an;
ring an=32003,(x,y,z),(C,dp);
ideal i=
zx2+y3,
xy2+z3,
yz2+x3,
x2y+y2z+z2x;
list I=mres(i,0);
I;
kill an;
ring cyc5 = 32003,(f,a,b,c,d,e),(C,dp);
ideal i=
a+b+c+d+e,
de+1cd+1bc+1ae+1ab,
cde+1bcd+1ade+1abe+1abc,
bcde+1acde+1abde+1abce+1abcd,
abcde-f5;
//std(i);
list I=mres(i,0);
I;
kill cyc5;
ring r=32003,(y,t,x,z,w),(C,dp);
ideal i=6*t^5*x^3*z^4+5*x^5*z^7+4*t^4*y*z^3*w^4,
5*t^3*x^2*y^7*z^4+5*t^6*x*y^3*z^3*w^3+4*t^5*x^5*y*w^5,
2*t^8*z^6+6*t^5*y*z^7*w+6*t^4*y^2*z^3*w^5;
//i;
//option(prot);
//ideal j=std(i);
//$
list I=mres(i,0);
I;
kill r;
ring cyc5 =32003,(a,b,c,d,e,f),(C,dp);
ideal i=
a5-b5,
b5-c5,
c5-d5,
d5-e5,
a4*b+b4*c+c4*d+d4*e+a*e4;
list I=mres(i,0);
I;
kill cyc5;
ring an=32003,(t,x,y,z,w),(C,dp);
ideal i=
2t3x5y2z+t5x2yw3+2x2yw8,
3t6x2yz2+9t3y2zw5+2xy3z2w5,
t3x5y2z2+tx2y3w6+2t5zw6;
list I=mres(i,0);
I;
kill an;
ring r=0,(x,y,z,t,w),(C,dp);
ideal i=6*t^5*x^3*z+5*x^5*z^4+4*t^4*y*w^4,
5*x*y^6*z^4+5*t^3*y^2*z^3*w^3+4*t^2*x^4*w^5,
2*t^4*z^3+6*t*y*z^4*w+6*y^2*w^5;
list I=mres(i,0);
I;
kill r;
tst_status(1);$;
