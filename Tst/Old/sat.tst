LIB "lib2";
ring r3=32003,(x(1..3)),ds;
ideal I1=x(2)^2-x(1)^4,x(3);
ideal I2=x(2),x(3)-x(1)^3;
ideal I=intersect(I1,I2);
I=std(I);
ideal J;
I;
for (int i=1;i<=size(I);i=i+1)
  {
    J=J,jet(I[i],ord(I[i]));
  }
J;
//option(yacc);
option();
ideal Jsat = sat(std(J),maxideal(1));
Jsat;
$

