LIB "tst.lib";
tst_init();

// raducal in alga.extensions
LIB "primdec.lib";
ring R=(0,a),(x,y,z),dp;
minpoly=a2+a+1;

poly L=x;
poly u1=1;
poly u2=1;
poly v1=y;
poly v2=z;
poly g1a=(a+2)/(-3)*(u1*v1*a-u2*v2);
poly g2a=(a+2)/(-3)*(u1*v1-u2*v2);
poly g1=L*g1a;
poly g2=L*g2a;
poly v3=g1a+(a+1)*g2a;
poly f1=(u1*u2*L^3+v1*v2*v3)/2;
poly f2=(-u1*u2*L^3+v1*v2*v3)/2;

poly F=g1^3-f1^2;
ideal I=jacob(F);
I=std(I);
hilb(I);

ideal I1=radical(I,"KL");
I1=std(I1);
hilb(I1);

ideal I2=radical(I);
I2=std(I2);
hilb(I2);

ideal I3=radicalEHV(I);
I3=std(I3);
hilb(I3);

// the Hilbert functions of the radicals must coincide:
hilb(I1,1);
hilb(I2,1);
hilb(I3,1);

tst_status(1);$
