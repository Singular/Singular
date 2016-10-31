LIB "tst.lib";
tst_init();

LIB"primdec.lib";
ring rv = (0,p),(c1,s4,s3,s2,s1),dp;
minpoly=p2-2;

poly p1=s1;
poly p2=c1^2-2*c1;
poly p3=s2*c1+(-4*p-4)*s3+(-2*p-4)*s2;
poly p4=s3*c1+(2*p+2)*s3+(p+2)*s2;
poly p5=s2^2;
poly p6=s3*s2;
poly p7=s3^2;
poly p8=s4+(-p-3)*s3+(-p)*s2;

ideal i0=p1,p2,p3,p4,p5,p6,p7,p8;
ideal i0g=groebner(i0);
size(i0g);
dim(i0g);
vdim(i0g);
radical(i0g);
dim(std(radical(i0g)));
vdim(std(radical(i0g)));

tst_status(1);$
