option(prot);
ring h1=32003,(t,x,y,z),dp;
ideal i=
9x8+y7t3z4+5x4y2t2+2xy2z3t2,
9y8+7xy6t+2x5y4t2+2x2yz3t2,
9z8+3x2y3z2t4;
intvec E=weight(i);
E;
ring r1 = 32003,(a,b,c,d,e,f),dp;
ideal i0 =
a+b+c+d+e+f,
ab+bc+cd+de+ef+fa,
abc+bcd+cde+d*ef+efa+fab,
abcd+bcde+cdef+defa+efab+fabc,
abcde+bcdef+cdefa+defab+efabc+fabcd,
abcdef+1;
ring rm=32003,(g,h,l,m,n,o),dp;
map f=r1,g,h,l3,m+n2,n,o2;
option(redSB);
ideal i=interred(f(i0));
i;
test(22);
intvec e=weight(i);
e;
test(-22);
ring sp= 32003,(t,x,y,z),dp; 
ideal i=t18x2-t19z-t18z2,t26xy-t27z-t25z3,t38y2-t37xy; 
intvec e=weight(i);
e;
test(22);
test(9);
ring al1=32003,(x,t,y,z),ds;
option(weightM);
ideal i=
9x8+y7t3z4+5x4y2t2+2xy2z3t2,
9y8+7xy6t+2x5y4t2+2x2yz3t2,
9z8+3x2y3z2t4;
ideal k=interred(i^2);
k;
degBound=1;
ideal j=std(k);
j;
kill al1;
LIB "tst.lib";tst_status(1);$
