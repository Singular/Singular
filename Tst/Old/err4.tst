//an error of 0.8.9a (intersect in char 0)
//(kreusler)
ring r=0,(x,y,z),(c,lp);
vector v(1)=[xy+y2,x,1];
vector v(2)=[x2+z2,x-y,0,1];
vector v(3)=[xz,y,0,0,1];
module M=v(1),v(2),v(3);
module Ms=std(M);
Ms;
module N=gen(3),gen(4),gen(5);
intersect(Ms,N);
kill r;
//--------------------------------------------------------------
//an error of 0.8.9a (undefined names output for block C)
ring r=0,x,(dp,C);
r;
kill r;
//--------------------------------------------------------------
//an error of 0.8.9a (crash: deg(0) was undefined)
// (B.Martin)
ring r=0,x,(dp,C);
poly f;
deg(f);
kill r;
//--------------------------------------------------------------
//an error of 0.8.9a (crash for gmp/i386/-O2)
// (B.Martin)
ring r=0,(x,y,z),(dp);
poly f=(x+y)^11;
subst(f,x,2);
kill r;
//---------------------------------------------------------------
//an error of 0.8.9b (incorrect) and 0.8.9c (crash)
// (H.S.)
ring r;
ideal i=1;
degree(i);
dim(i);
mult(i);
i=0;
degree(i);
dim(i);
mult(i);
//---------------------------------------------------------------
//an error of 0.8.9c (crash)
// (Cottbus)
//
// test script for var command
//
ring r4=0,(x(1..10),y(1..9),z(1..8)),dp;
r4;
varstr(r4,28);
kill r4;
// -----------------------------------------------------------
//an error of 0.8.9c (crash/undef output)
// (Cottbus)
//
// test script for nameof command
//
int i=9;
string st=nameof(i+1);
st;
// -----------------------------------------------------------
//an error of 0.8.9c (crash for det of nonsquare)
// GMG
ring r;
ideal I =x,y,z,zy;
matrix m =jacob(I);
det(m);
kill r;
// -----------------------------------------------------------
//an error of 0.8.9c (crash if proc paramter is a ring)
// GMG
ring r;
r;
r;
proc t
{
listvar(all);
;
}
t(r);
listvar(all);
r;
kill r;kill t;
// -----------------------------------------------------------
// test automatic set of options for various rings
// GP/HS
option();
option(intStrategy);
option();
option(nointStrategy);
option();
ring r=32003,x,dp;
r;
option();
option(intStrategy);
option();
option(nointStrategy);
option();
kill r;
ring r=0,x,dp;
r;
option();
option(intStrategy);
option();
option(nointStrategy);
option();
kill r;
ring r=(0,a),x,dp;
r;
option();
option(intStrategy);
option();
option(nointStrategy);
option();
kill r;
ring r=(7,a),x,dp;
r;
option();
option(intStrategy);
option();
option(nointStrategy);
option();
kill r;
// -----------------------------------------------------------
// could not display number from lists (sleftv::String())
// OB/HS
ring r=0,x,dp;list l=1/2;l[1];
LIB "tst.lib";tst_status(1);$

