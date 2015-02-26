LIB "tst.lib";
tst_init();

// tr.590 ------------------------------------------------------------------------------------
proc f;
f; // was: NULL deref which printing internal structure

// tr.588 ------------------------------------------------------------------------------------
// the functions syz, intersect, quotient return a standard base instead of a generating set if returnSB is set.
option(returnSB);
ring r=32003,(x,y,z),dp;

module m=syz(ideal(x+y,x+z));
attrib(m);
m;
std(m);

ideal i=intersect(ideal(x+y),ideal(x2+z,y3));
i;
attrib(i);
i=intersect(ideal(x+y),ideal(x2+z,y3),ideal(y));
i;
attrib(i);
std(i);

i=quotient(ideal(x+y,z5),ideal(z));
i;
attrib(i);
i=quotient(ideal(x+y,z5),ideal(y,z));
i;
attrib(i);

ideal h1=x,y,z;
ideal h2=x;
module m=modulo(h1,h2);
print(m);
attrib(m);
print(std(m));

// tr.579 -----------------------------------------------------------------------------------------
// trying dumb things with minpoly
ring rng = (0,a),x,dp;
minpoly = a^2-1;
minpoly = a^2-2;
minpoly = a^2-2;

// tr.576 -----------------------------------------------------------------------------------------
// trying dumb things with minpoly (was: refering to undef memory)
ring rng = 5^5,x,dp;
ring rng = 5^2,x,dp;
ring rng = 25,x,dp;
ring rng = (integer,5^5),x,dp;
basering;
noether=1;
ring rng = (integer,5^2),x,dp;
basering;
minpoly;
noether;
ring rng = (integer,5,2),x,dp;
ring rng = 5^5,x,dp;
ring rng = 5^2,x,dp;
ring rng = 25,x,dp;
ring rng = (integer,5^5),x,dp;
basering;
ring rng = (integer,5^2),x,dp;
basering;
minpoly;
ring rng = (integer,5,2),x,dp;

tst_status(1);$

