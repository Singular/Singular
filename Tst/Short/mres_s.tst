LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");

proc tst_test_mres(ideal i)
{
   resolution rs = mres(i, 0);
   rs;
   list(rs);
   betti(rs);
}


ring an=32003,(w,x,y,z),(dp,c);
ideal i=
wx2+y3,
xy2+z3,
yz2+w3,
zw2+x3,
xyz+yzw+zwx+wxy;
tst_test_mres(i);
kill an;

ring an=32003,(w,x,y,z),(dp,c);
ideal i=
wx+y2,
xy+z2,
yz+w2,
zw+x2,
xy+yz+zw+wx;
tst_test_mres(i);
kill an;



ring an=32003,(x,y,z),(dp,c);
ideal i=
zx2+y3,
xy2+z3,
yz2+x3,
x2y+y2z+z2x;
tst_test_mres(i);
kill an;

ring an=32003,(w,x,y,z),(dp,C);
ideal i=
1w2xy+1w2xz+1w2yz+1wxyz+1x2yz+1xy2z+1xyz2,
1w4x+1w4z+1w3yz+1w2xyz+1wx2yz+1x2y2z+1xy2z2,
1w6+1w5z+1w4xz+1w3xyz+1w2xy2z+1wx2y2z+1x2y2z2;
tst_test_mres(i);
kill an;

ring an=0,(w,x,y,z),(dp,C);
ideal i=
1w2xy+1w2xz+1w2yz+1wxyz+1x2yz+1xy2z+1xyz2,
1w4x+1w4z+1w3yz+1w2xyz+1wx2yz+1x2y2z+1xy2z2,
1w6+1w5z+1w4xz+1w3xyz+1w2xy2z+1wx2y2z+1x2y2z2;
tst_test_mres(i);
kill an;

ring an=0,(w,x,y,z),(dp,c);
ideal i=
wx2+y3,
xy2+z3,
yz2+w3,
zw2+x3,
xyz+yzw+zwx+wxy;
tst_test_mres(i, 1);
kill an;

ring an=0,(w,x,y,z),(dp,c);
ideal i=
wx+y2,
xy+z2,
yz+w2,
zw+x2,
xy+yz+zw+wx;
tst_test_mres(i);
kill an;

ring an=0,(x,y,z),(dp,c);
ideal i=
zx2+y3,
xy2+z3,
yz2+x3,
x2y+y2z+z2x;
tst_test_mres(i);
kill an;

tst_status(1);$
