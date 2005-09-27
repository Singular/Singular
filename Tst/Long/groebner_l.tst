LIB "tst.lib";
tst_init();

option(prot);
//------------------------------------------------------------

ring R=(0,w),(x1,y1,z1,x2,y2,z2,x3,y3,z3,l,d,xs,ys,zs,x,y,z), Dp;
minpoly=w^2-3;
poly s1=x1^2+y1^2+(2-z1)^2-9;
poly s2=(-w-x2)^2+y2^2+(-1-z2)^2-9;
poly s3=(w-x3)^2+y3^2+(-1-z3)^2-9;
poly s4=(x-x1)*x+(y-y1)*y+(z-z1)*z;
poly s5=(x-x2)*x+(y-y2)*y+(z-z2)*z;
poly s6=(x-x3)*x+(y-y3)*y+(z-z3)*z;
poly s7=(x1-x2)^2+(y1-y2)^2+(z1-z2)^2-12;
poly s8=(x3-x2)^2+(y3-y2)^2+(z3-z2)^2-12;
poly s9=(x1-x3)^2+(y1-y3)^2+(z1-z3)^2-12;
poly s10=(x1+x2+x3)-3*x;
poly s11=(y1+y2+y3)-3*y;
poly s12=(z1+z2+z3)-3*z;
poly s13=x^2-xs^2;
poly s14=y^2-ys^2;
poly s15=z^2-zs^2;
poly s16=x^2+y^2+z^2-l^2;
poly s17=y^2+z^2-d^2;
poly s18=x-x1;
poly s19=-3*d*ys-zs*6-3*d*y1;
poly s20=-3*d*zs+ys*6-3*d*z1;
poly s21=-2*l*xs+d*2*w+2*l*x2;
poly s22=-6*ys*xs*w-6*d*l*ys+6*zs*l-6*d*l*y2;
poly s23=-6*zs*xs*w-6*zs*d*l-6*ys*l-6*d*l*z2;
poly s24=2*w*d+2*xs*l-2*l*x3;
poly s25=6*w*ys*xs-6*ys*d*l+6*zs*l-6*d*l*y3;
poly s26=6*w*zs*xs-6*zs*d*l-6*ys*l-6*d*l*z3;
ideal I=s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26;
ideal J=groebner(I);
J; 
kill R;

tst_status(1);$
