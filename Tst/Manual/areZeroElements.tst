LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z),dp;
intmat S[2][3]=
1,0,1,
0,1,1;
intmat L[2][1]=
2,
2;
setBaseMultigrading(S,L);
poly a = 1;
poly b = xyz;
ideal I = a, b;
print(multiDeg(I));
intmat m[5][2]=multiDeg(a),multiDeg(b); m=transpose(m);
print(multiDeg(a));
print(multiDeg(b));
print(m);
areZeroElements(m);
intmat LL[2][1]=
1,
-1;
areZeroElements(m,LL);
tst_status(1);$
