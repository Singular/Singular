LIB "tst.lib";
tst_init();

LIB "paraplanecurves.lib";
ring R=0,(x,y,z),dp;

poly f=x*(y^5+z^5)-x^4*y*z+(x*y*z)^2-2*(y*z)^3;
ideal actual=adjointIdeal(f,1);
ideal expected=
  x^2*y-x*y^2-2*y^3+x^2*z-y^2*z+2*x*z^2-y*z^2+z^3,
  x*y^2+y^3-x*z^2-z^3,
  y^3-x^2*z-x*z^2+y*z^2,
  x^2*z-y*z^2;

int equal=tst_stdEqual(std(actual),std(expected));
ASSUME(0,equal);
equal;

list twoReps;
twoReps[1]=list(y,1,0,0,0,list(1));
twoReps[2]=list(y+x,1,0,0,0,list(1));
int representativesPreserved=
  (size(getClasses(twoReps,1))==2 and size(getClasses(twoReps))==1);
ASSUME(0,representativesPreserved);
representativesPreserved;

LIB "integralbasis.lib";
ring Rib=0,(x,y),dp;
poly g=(x+y)*(y^5+1)-(x+y)^4*y+((x+y)*y)^2-2*y^3;
list ib=integralBasis(g,2,"isIrred","noOpti");
poly expectedDen=x5-5x2+5x;
ideal expectedIB=
  x5-5x2+5x,
  x5y-5x2y+5xy,
  x5y2-5x2y2+5xy2,
  x5y3-5x2y3+5xy3,
  x5y4-5x2y4+5xy4,
  5x4y4-3x4y3+3x3y4-6x4y2-4x3y3+4x2y4-9x4y-8x3y2
    -9x2y3-16xy4+11y5+x4-23x3y-51x2y2-30xy3+5x3
    +6x2y+50xy2+11y3+3x2+42xy-11y2-x-11y;
int integralBasisOK=
  (size(ib[1])==6 and ib[2]==expectedDen
   and tst_stdEqual(std(ib[1]),std(expectedIB)));
ASSUME(0,integralBasisOK);
integralBasisOK;

tst_status(1);$
