LIB "tst.lib";
tst_init();

LIB "symodstd.lib";

example genSymId;

example isSymmetric;

example primRoot;

example eigenvalues;

intvec pi, tau, sigma;

ring R1 = 0, x(1..4), dp;
ideal I = cyclic(4);
pi = 2,3,4,1;
symmStd(I,pi);

ring R2 = 31, (x,y,z), dp;
ideal J;
J[1] = xy-y2+xz;
J[2] = xy+yz-z2;
J[3] = -x2+xz+yz;
tau = 3,1,2;
symmStd(J,tau);

ring R3 = 30817, x(1..8), dp;
ideal I = cyclic(8);
sigma = 2,3,4,5,6,7,8,1;
symmStd(I,sigma);

ring R4 = 0, (x,y,z), dp;
ideal I;
I[1] = -2xyz4+xz5+xz;
I[2] = -2xyz4+yz5+yz;
pi = 2,1,3;
syModStd(I,pi);

ring R5 = 0, x(1..4), dp;
ideal I = cyclic(4);
tau = 2,3,4,1;
ideal sJ1 = syModStd(I,tau,3);
ideal sJ2 = syModStd(I,tau,1,1);
size(reduce(sJ1,sJ2));
size(reduce(sJ2,sJ1));

ring R6 = 0, (a(1..3),b(1..3),c(1..2)), dp;
ideal I;
I[1]=4*a(1)^5*a(2)+2*b(1)*b(2)*b(3)*c(2);
I[2]=4*a(1)^5+20*a(1)^4*a(2)+3*b(1)*b(2)*b(3)*c(1)+b(1)*b(2)*c(2)-4*b(2)*b(3)*c(2);
I[3]=20*a(1)^4+40*a(1)^3*a(2)+4*b(1)*b(2)*b(3)+2*b(1)*b(2)*c(1)+b(1)*b(3)*c(1)-3*b(2)*b(3)*c(1)-b(1)*c(2)-5*b(2)*c(2)-6*b(3)*c(2);
I[4]=40*a(1)^3+40*a(1)^2*a(2)+5*b(1)*b(2)*b(3)+3*b(1)*b(2)+2*b(1)*b(3)-2*b(2)*b(3)-4*b(2)*c(1)-5*b(3)*c(1)-7*c(2);
I[5]=40*a(1)^2+20*a(1)*a(2)+4*b(1)*b(2)+3*b(1)*b(3)-b(2)*b(3)+b(1)-3*b(2)-4*b(3)-6*c(1);
I[6]=20*a(1)+4*a(2)+2*b(1)-2*b(2)-3*b(3)-5;
I[7]=4*b(1)^5*b(2)+2*a(1)*a(2)*a(3)*c(2);
I[8]=4*b(1)^5+20*b(1)^4*b(2)+3*a(1)*a(2)*a(3)*c(1)+a(1)*a(2)*c(2)-4*a(2)*a(3)*c(2);
I[9]=20*b(1)^4+40*b(1)^3*b(2)+4*a(1)*a(2)*a(3)+2*a(1)*a(2)*c(1)+a(1)*a(3)*c(1)-3*a(2)*a(3)*c(1)-a(1)*c(2)-5*a(2)*c(2)-6*a(3)*c(2);
I[10]=5*a(1)*a(2)*a(3)+40*b(1)^3+40*b(1)^2*b(2)+3*a(1)*a(2)+2*a(1)*a(3)-2*a(2)*a(3)-4*a(2)*c(1)-5*a(3)*c(1)-7*c(2);
I[11]=4*a(1)*a(2)+3*a(1)*a(3)-a(2)*a(3)+40*b(1)^2+20*b(1)*b(2)+a(1)-3*a(2)-4*a(3)-6*c(1);
I[12]=2*a(1)-2*a(2)-3*a(3)+20*b(1)+4*b(2)-5;
sigma = 4,5,6,1,2,3,7,8;
ideal sI1 = syModStd(I,sigma);
ideal sI2 = modStd(I);
size(reduce(sI1,sI2));
size(reduce(sI2,sI1));

tst_status(1);$
