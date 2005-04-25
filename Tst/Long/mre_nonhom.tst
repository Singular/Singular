LIB "tst.lib";
tst_init();
option(prot);
ring r;
ideal i=xy2+yz2+xz2,x3+x3z,y3,z3;
list I=mres(i,0);
I;
kill r;
ring an=32003,(w,x,y,z),(dp,c);
ideal i=
wx2+y4,
xy2+z3,
yz2+w3,
zw2+x3,
xyz+yzw+zwx+wxy;
list I=mres(i,0);
I;
kill an;
ring an=32003,(w,x,y,z),(dp,c);
ideal i=
wx+y3,
xy+z2,
yz+w2,
zw+x2,
xy+yz+zw+wx;
list I=mres(i,0);
I;
kill an;
ring an=32003,(x,y,z),(dp,c);
ideal i=
zx2+y4,
xy2+z4,
yz2+x4,
x2y+y2z+z2x;
list I=mres(i,0);
I;
kill an;
ring cyc5 = 32003,(f,a,b,c,d,e),dp;
ideal i=
a+b+c+d+e,
de+1cd+1bc+1ae+1ab+abcde,
cde+1bcd+1ade+1abe+1abc,
bcde+1acde+1abde+1abce+1abcd;
//std(i);
list I=mres(i,0);
I;
kill cyc5;
ring cyc5 =32003,(a,b,c,d,e,f),dp;
ideal i=
a4-b5,
b4-c5,
c4-d5,
d4-e5,
a3*b+b3*c+c3*d+d3*e+a*e3;
list I=mres(i,0);
I;
kill cyc5;
tst_status(1);$;
