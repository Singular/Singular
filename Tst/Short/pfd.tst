LIB "tst.lib";
tst_init();

LIB "pfd.lib";

example pfd;

// pfd(poly,poly) //////////////////////////////////////////////////////////////
ring r1 = 0, x(1..5), dp;
poly f = -2*x(1)*x(3)+3*x(3)*x(4)+x(2)*x(5)+x(3)*x(5)-x(4)*x(5);
poly g = x(1)*x(3)*(-x(2)+x(3)+x(4))*(x(1)-x(5))*x(5);
list dec = pfd(f,g);
displaypfd(dec);
getStringpfd_indexed(dec);
displaypfd_long(dec);
getStringpfd(dec);
checkpfd(list(f,g), dec);
checkpfd(list(f,g), dec,10);
kill dec;

ring r2 = (0,i), (q,r,s,t), dp;
minpoly = i^2+1;
poly f = 5*i*q^2*r + s^3*r - r^4 + (1+i)*q*r*s*t + s^2*t^2 -t^3;
poly g = 4/(1+i)*q*r^3*(i*r+s-t^2)^2*(q^2+r^2+s^2);
list dec = pfd(f,g);
displaypfd(dec);
getStringpfd_indexed(dec);
displaypfd_long(dec);
getStringpfd(dec);
checkpfd(list(f,g), dec);
checkpfd(list(f,g), dec,10);
kill dec;


// pfd(poly,list) (denominator = ideal of factors & intvec of exponents) ///////
ring r3 = 5, (x,y,z), dp;
poly f = x+y+z+1;
list g = list(ideal((x^2+y^2+z^2),(x+y^2),(y+z^2),(z+x^2)), intvec(2,1,1,1));
list dec = pfd(f,g);
displaypfd(dec);
checkpfd(list(f,g), dec);
checkpfd(list(f,g), dec, 10);
kill dec;

// different ordering, same polynomials:
ring r4 = 5, (x,y,z), lp;
poly f = fetch(r3,f);
list g = fetch(r3,g);
list dec = pfd(f,g);
displaypfd(dec);
checkpfd(list(f,g), dec);
checkpfd(list(f,g), dec,10);
kill dec;


// pfd(list) ///////////////////////////////////////////////////////////////////
ring s1 = 0, (x,y,z), dp;
poly f1 = x*y+y*z+z*x-x-y-z+1;
poly g1 = (z-2)*x^2*(y+1)^2*(x+2*y^2-z);
poly f2 = 1;
poly g2 = (z-2)*x^2*(y+1)^2*(x+2*y^2-z);
poly f3 = x^4+y^4+z^4;
poly g3 = (x^2+y^2+z^2)^2*(x+y+z)^3;
list list_of_args = list(list(f1,g1), list(f2,g2), list(f3,g3));
list dec = pfd(list_of_args);
displaypfd(dec[1]);
displaypfd(dec[2]);
displaypfd(dec[3]);
checkpfd(list(f1,g1), dec[1]);
checkpfd(list(f2,g2), dec[2]);
checkpfd(list(f3,g3), dec[3]);
kill dec;

// pfd(matrix) /////////////////////////////////////////////////////////////////
ring s2 = 3, (x,y,z), dp;
poly f11 = (x+y+z+1)^3;
poly g11 = (x-y)*(y-z)^2*(z-x)^3*(x^2+y^2+z^2);
poly f12 = 1;
poly g12 = (x-y)*(y-z)^2*(z-x)^3*(x^2+y^2+z^2);
poly f21 = (x+2*y+3*z)^3-10;
poly g21 = (x-y)^3*(y-z)^3*(z-x)^3*(x^2+y^2+z^2)^2;
poly f22 = 1;
poly g22 = (x-y)^3*(y-z)^3*(z-x)^3*(x^2+y^2+z^2)^2;
list matrix_of_args = list(list(list(f11,g11), list(f12,g12)),
                           list(list(f21,g21), list(f22,g22)));
list dec = pfd(matrix_of_args);
displaypfd(dec[1][1]);
displaypfd(dec[1][2]);
displaypfd(dec[2][1]);
displaypfd(dec[2][2]);
checkpfd(list(f11,g11), dec[1][1]);
checkpfd(list(f12,g12), dec[1][2]);
checkpfd(list(f21,g21), dec[2][1]);
checkpfd(list(f22,g22), dec[2][2]);
kill dec;


tst_status(1);$
