option(prot);
ring r;
ideal i=x2+y3,xyz+z4+y2z2,y3+x3z;
qring q=std(i);
q;
ideal i=x,y,z;
list I=sres(i,0);
size(I);
I;
$
