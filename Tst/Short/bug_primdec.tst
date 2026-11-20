LIB "primdec.lib";
ring  r = 32003,(x,y,z),dp;
poly  p = z2+1;
poly  q = z4+2;
ideal i = p^2*q^3,(y-z3)^3,(x-yz+z4)^4;
list pr= prepareAss(i);
pr;

////////////////////////////////////////////////////////////////
// test that one more 'is no standard basis' warning is gone away
system("random",1452807367);
proc primdecMinpolyExample()
{
    ring rng = (7,vv),(x,y,z,u),dp;
    minpoly =  vv^2+vv+3;
    ideal I = (-3*vv-1)*u^2+2,2*x*z+(2*vv-2)*x+(2*vv+1),(vv+2)*x^2*u+(2*vv+1)*z^2*u+(vv)*z*u^2;
    list result = minAssGTZ (I);
}

// increase call depth:
proc testPrimdecMinpolyExample()  {    return( primdecMinpolyExample() );   }
testPrimdecMinpolyExample();

// check that redefinig gnir1 is gone away:
system("random",1463736066);
ring rng2 = (0),(x,y,z,u,w),dp;
ideal I2 = -28*1*x*z*w*1-15*y*z*u*w,-28*1^2*x*u+14*1*x^2*y*w*1+17,12*1*u*w^2*1+18*x*y^2*w;
list result2 = primdecGTZ (I2);

////////////////////////////////////////////////////////////////
// avoid the two-parameter performance regression from Oscar issue 6421
ring issue6421_r = (0,a,b),(x,y,z),dp;
poly h = (x+2)*(z+2);
ideal issue6421_i =
  x*(z+2)*((1-x-b)*(x+2)-(a+1)*y),
  y*(-a*h+2*(a+1)*x),
  z*((a+1)*x*y-h);
option(noredSB);
intvec issue6421_options = option(get);
list issue6421_primes = minAssGTZ(issue6421_i);
size(issue6421_primes)==7;
option(get)==issue6421_options;

// These inputs expose redSB in the returned basis representation.
// Reset the random seed so that only the selected option differs.
ideal issue6421_override_i = (x+a*y)*(x+b*z), y*z;
system("random", 12345678);
list issue6421_default = minAssGTZ(issue6421_override_i);
option(get)==issue6421_options;
system("random", 12345678);
list issue6421_red = minAssGTZ(issue6421_override_i, "redSB");
option(get)==issue6421_options;
string(issue6421_default)==string(issue6421_red);
option(redSB);
intvec issue6421_red_options = option(get);
system("random", 12345678);
list issue6421_nored = minAssGTZ(issue6421_override_i, "noredSB");
option(get)==issue6421_red_options;
string(issue6421_red)!=string(issue6421_nored);

ring issue6421_one_parameter = (0,a),(x,y,z),dp;
ideal issue6421_one_i = (x+a*y)*(x+(a+1)*z), y*z;
option(noredSB);
intvec issue6421_one_options = option(get);
system("random", 12345678);
list issue6421_one_default = minAssGTZ(issue6421_one_i);
option(get)==issue6421_one_options;
system("random", 12345678);
list issue6421_one_red = minAssGTZ(issue6421_one_i, "redSB");
option(get)==issue6421_one_options;
string(issue6421_one_default)!=string(issue6421_one_red);

ring issue6421_no_parameters = 0,(x,y,z),dp;
ideal issue6421_zero_i =
  -3x2-xy+4xz+4yz+3z2-4x+y-2z-2,
  -4y2+3xz+3yz+2z2+3x+y+5z-5;
option(redSB);
intvec issue6421_zero_options = option(get);
system("random", 12345678);
list issue6421_zero_default = minAssGTZ(issue6421_zero_i);
option(get)==issue6421_zero_options;
system("random", 12345678);
list issue6421_zero_red = minAssGTZ(issue6421_zero_i, "redSB");
option(get)==issue6421_zero_options;
string(issue6421_zero_default)==string(issue6421_zero_red);
system("random", 12345678);
list issue6421_zero_nored = minAssGTZ(issue6421_zero_i, "noredSB");
option(get)==issue6421_zero_options;
string(issue6421_zero_red)!=string(issue6421_zero_nored);

ring issue6421_local = (0,a),(x,y,z),ds;
ideal issue6421_local_i = (x+a*y)*(x+(a+1)*z), y*z;
option(noredSB);
intvec issue6421_local_options = option(get);
system("random", 12345678);
list issue6421_local_red = minAssGTZ(issue6421_local_i, "redSB");
option(get)==issue6421_local_options;
system("random", 12345678);
list issue6421_local_nored = minAssGTZ(issue6421_local_i, "noredSB");
option(get)==issue6421_local_options;
string(issue6421_local_red)!=string(issue6421_local_nored);


LIB "tst.lib"; tst_status(1);
$
