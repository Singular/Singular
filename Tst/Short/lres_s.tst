LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
ring an=0,(w,x,y,z),(dp,C);
ideal i=
1w2xy+1w2xz+1w2yz+1wxyz+1x2yz+1xy2z+1xyz2,
1w4x+1w4z+1w3yz+1w2xyz+1wx2yz+1x2y2z+1xy2z2,
1w6+1w5z+1w4xz+1w3xyz+1w2xy2z+1wx2y2z+1x2y2z2;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill an;
ring an=0,(w,x,y,z),(dp,c);
ideal i=
wx2+y3,
xy2+z3,
yz2+w3,
zw2+x3,
xyz+yzw+zwx+wxy;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill an;
ring an=0,(w,x,y,z),(dp,c);
ideal i=
wx+y2,
xy+z2,
yz+w2,
zw+x2,
xy+yz+zw+wx;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill an;
ring an=0,(x,y,z),(dp,c);
ideal i=
zx2+y3,
xy2+z3,
yz2+x3,
x2y+y2z+z2x;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill an;
ring cyc5 = 32003,(a,b,c,d,e,f),dp;
ideal i=
a+b+c+d+e,
de+1cd+1bc+1ae+1ab,
cde+1bcd+1ade+1abe+1abc,
bcde+1acde+1abde+1abce+1abcd,
abcde-f5;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill cyc5;
tst_status(1);$
