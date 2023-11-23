LIB "tst.lib";
tst_init();

//EXAMPLES SHEET
LIB"JMSConst.lib";

//Example 1
ring r=0, (x,y,z),ip;
ideal Borid=z^5,z^4*y,z^3*y^2,z^2*y^3,z^4*x, z^3*y*x, z^2*y^2*x, z*y^4,z*y^3*x, z^3*x^2, z^2*y*x^2, z*y^2*x^2, y^5;
JMarkedScheme(Borid,r);
//152 equations

//Example 2
ring r=0, (x,y,z),ip;
ideal Borid=z^4*y,z^3*y^2,z^2*y^3,z^4*x, z^3*y*x, z^2*y^2*x, z*y^4,z*y^3*x, z^3*x^2, z^2*y*x^2, z*y^2*x^2, y^5;
JMarkedScheme(Borid,r);
//In this example Borid is not a Borel ideal, so the procedure returns the expected error message and the result is the empty list


//Example 3
ring r=0, (x(0..3)), ip;
ideal Borid=x(3)^2, x(3)*x(2),x(2)^2,x(3)*x(1),x(2)*x(1),x(1)^3;
JMarkedScheme(Borid,r);
//40 equations

//Example 4
ring r=0, (x,y,z),ip;
ideal Borid=xy,y^2,xz,yz,z^2;
JMarkedScheme(Borid,r);
//6 equations


//Example 5
ring r=0, (z,y,x),ip;
ideal Borid=x^4,x^3*y,x^2*y^2,x*y^3,z*x^3,x^2*y*z, x*y^2*z,y^5;
JMarkedScheme(Borid,r);
// 80 equations


//Example 6
ring r=0, (z,y,x),ip;
ideal Borid=yz,z^2,x*y^2,y^3,x^4,x^3*y,x^3*z;
JMarkedScheme(Borid,r);
// This is not a Borel ideal.


//Example 7
ring r=0, (x,y,z),ip;
ideal Borid=z,y^6,x^3*y^5;
JMarkedScheme(Borid,r);
// 10 equations.

//Example 8
ring r=0, (x,y,z),ip;
ideal Borid=yz, z^2, y^3, xy^2;
JMarkedScheme(Borid,r);
//13 equations

//Example 9
ring r=0, (x,y,z),ip;
ideal Borid=yz, z^2, y^3, x^2*z;
JMarkedScheme(Borid,r);
// 12 equations

//Example 10
ring r=0,(x,z,y),ip;
ideal Borid=yz, z^2, y^3, x^2*z;
JMarkedScheme(Borid,r);
// This is not a Borel ideal.

//Example 11
ring r=0,(z,y,x),ip;
ideal Borid=x^2,xy,y^2,xz;
JMarkedScheme(Borid,r);
// 8 equations.

//Example 12
ring r=0,(w,z,y,x),ip;
ideal Borid=x^2,xy,y^2,xz,yz,z^3,xw^2,yw^2;
JMarkedScheme(Borid,r);
// 55 equations.

//Example 13
ring r=0, (x(0..5)),ip;
ideal Bu=x(1)^2*x(2),x(1)^2*x(3),x(1)^2*x(4),x(1)^2*x(5),x(0)*x(2)^2,x(1)*x(2)^2,x(2)^3,x(2)^2*x(3),x(2)^2*x(4),x(2)^2*x(5),x(0)*x(2)*x(3),x(1)*x(2)*x(3),x(2)*x(3)^2,x(2)*x(3)*x(4),x(2)*x(3)*x(5),x(0)*x(3)^2,x(1)*x(3)^2,x(3)^3,x(3)^2*x(4),x(3)^2*x(5),x(0)*x(2)*x(4),x(1)*x(2)*x(4),x(2)*x(4)^2,x(2)*x(4)*x(5),x(0)*x(3)*x(4),x(1)*x(3)*x(4),x(3)*x(4)^2,x(3)*x(4)*x(5),x(0)*x(4)^2,x(1)*x(4)^2,x(4)^3,x(4)^2*x(5),x(0)*x(2)*x(5),x(1)*x(2)*x(5),x(2)*x(5)^2,x(0)*x(3)*x(5),x(1)*x(3)*x(5),x(3)*x(5)^2,x(0)*x(4)*x(5),x(1)*x(4)*x(5),x(4)*x(5)^2,x(0)*x(5)^2,x(1)*x(5)^2,x(5)^3;
list L=sort(Bu);
ideal Borid=L[1]+ideal(x(1)^4);
JMarkedScheme(Borid,r);
// 1860 equations. This is not really fast, requiring 2 or 3 minutes.



//Example 14
ring r=0,(z,y,x),ip;
ideal Borid=x^2,xy,y^3,xz^3;
JMarkedScheme(Borid,r);
// 12 equations.


//Example 15
ring r=0,(x,y),ip;
ideal Borid=xy,y^2;
JMarkedScheme(Borid,r);
// 1 equation, namely  (-c(2)-c(1)^2).

example BorelCheck;
example JMarkedScheme;

tst_status(1);$
