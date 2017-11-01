LIB "tst.lib";
tst_init();

LIB "realclassify.lib";

ring r = 0, (x,y,z), ds;
poly f = (x2+3y-2z)^2+xyz-(x-y3+x2z3)^3;
realmorsesplit(f);

ring s = 0, (x,y), ds;
poly f = x3+y4;
milnornumber(f);

poly g = x3+xy3;
determinacy(g);


ring R = 0, (x,y),ls;
map phi = R, x+x*y+y^2+x^2+x^2*y+y^3,y+y*x+y^2*x+y^3+y^4+x^2+x^3;




poly p = x^2*y+y^3;  // D4+
p = phi(p);
realclassify(p);


poly p = x^2*y-y^3;  // D4-
p = phi(p);
realclassify(p);
basering;

poly p = x^2*y+y^7;  // D8+
p = phi(p);
realclassify(p);


poly p = x^2*y-y^7;  // D8-
p = phi(p);
realclassify(p);

poly p = x^3+y^4;  // E6+
p = phi(p);
realclassify(p);


poly p = x^3-y^4;  // E6-
p = phi(p);
realclassify(p);

poly p = x^3+x*y^3;  // E7
p = phi(p);
realclassify(p);

poly p = x^3+y^5;  // E8
p = phi(p);
realclassify(p);

poly p = x^3-2*x*y^4;  // J10
p = phi(p);
realclassify(p);

poly p = x^3+3*y^6;  // J10   n=1;
p = phi(p);
realclassify(p);

poly p = x^3+x*y^4;  // J10   n=1;
p = phi(p);
milnornumber(p);
realclassify(p);


poly p = x^3-3*x*y^4-y^6;  // J10  n=3
p = phi(p);
realclassify(p);

poly p = x^3-3*x*y^4+y^6;  // J10  n=3
p = phi(p);
realclassify(p);

poly p = x^3+10*x^2*y^2-x*y^4;  // J10  n=3
p = phi(p);
realclassify(p);

poly f  = x^3+x^2*y^2+4*y^9;  //J[13]+
f=phi(f);
realclassify(f);

poly f  = x^3-3*x^2*y^2+4*y^9;  //J[13]-
f=phi(f);
realclassify(f);


poly f  = -x^4-3*x^2*y^2+3*y^9;  //X[14]--
f=phi(f);
realclassify(f);

poly f  = x^4-x^2*y^2+3*y^9;  //X[14]+-
f=phi(f);
realclassify(f);

poly f  = -x^4+x^2*y^2+3*y^9;  //X[14]-+
f=phi(f);
realclassify(f);

poly f  = x^4+x^2*y^2+3*y^9;  //X[14]++
f=phi(f);
realclassify(f);


poly f  = x^3+y^7+3*x*y^5;  // E[12]
f=phi(f);
realclassify(f);

poly f  = 3*x^3+y^7+3*x*y^5;  // E[12]
f=phi(f);
realclassify(f);

poly f  = x^3+x*y^5+4*y^8;  // E[13]
f=phi(f);
realclassify(f);

poly f  = 3*x^3+x*y^5+4*y^8;  // E[13]
f=phi(f);
realclassify(f);

poly f  = x^3+y^8+2*x*y^6;  // E[14]+
f=phi(f);
realclassify(f);

poly f  = 3*x^3-y^8+2*x*y^6;  // E[14]-
f=phi(f);
realclassify(f);

poly f  = x^3*y+y^5+5*x*y^4;  // Z[11]
f=phi(f);
realclassify(f);


poly f  = 3*x^3*y+y^5+5*x*y^4;  // Z[11]
f=phi(f);
realclassify(f);

poly f  = x^3*y+x*y^4+6*x^2*y^3;  // Z[12]
f=phi(f);
realclassify(f);

poly f  = 3*x^3*y+x*y^4+6*x^2*y^3;  // Z[12]
f=phi(f);
realclassify(f);

poly f  = x^3*y-y^6+2*x*y^5;  // Z[13]-
f=phi(f);
realclassify(f);

poly f  = 3*x^3*y+y^6+2*x*y^5;  // Z[13]+
f=phi(f);
realclassify(f);

poly f  = x^4+y^5+x^2*y^3;  // W[12]+
f=phi(f);
realclassify(f);

poly f  = -3*x^4+y^5+x^2*y^3;  // W[12]-
f=phi(f);
realclassify(f);

poly f  = -x^4+x*y^4+y^6;  // W[13]-
f=phi(f);
realclassify(f);

poly f  = 3*x^4+x*y^4+y^6;  // W[13]+
f=phi(f);
realclassify(f);

poly p = 5*x^2*y^2+x^7+y^7; // Y7,7
p=phi(p);
realmorsesplit(p,7);
realclassify(p);

poly p = 5*x^2*y^2+x^6+y^7; // Y6,7
p = phi(p);
realclassify(p);

poly p = -3*x^2*y^2+2*x^6+y^8; // Y6,8
p = phi(p);
realclassify(p);


poly p = -x^2*y^2-x^6-y^8; // Y6,8
p = phi(p);
realclassify(p);

poly p = x^2*y^2+x^5+y^9; // Y5,9
p = phi(p);
realclassify(p);

poly p = 5*(x^2-2*y^2)^2+y^7;
p = phi(p);
realclassify(p);

poly p = (x^2-2*y^2)^2+y^8;
p = phi(p);
realclassify(p);

poly p = 5*(x^2+y^2)^2+y^7; // \tildeY7
p = phi(p);
realclassify(p);

poly p = 5*(x^2+y^2)^2+y^8; // \tildeY8
p = phi(p);
realclassify(p);

//case X9
poly p = x^4-x^2*y^2+5*y^4+x*y^3+x^3*y;
realclassify(p);
poly p = -x^4-x^2*y^2+5*y^4+x*y^3;
realclassify(p);
poly p = x^4-x^2*y^2-5*y^4+x*y^3;
realclassify(p);
poly p = -x^4-x^2*y^2-5*y^4+x*y^3;
realclassify(p);


  ring r = 0, (x,y,z), ds;
  poly f = (x2+3y-2z)^2+xyz-(x-y3+x2z3)^3;
  list NFR = realclassify(f);
  NFR;
  addnondegeneratevariables(NFR[1]);



tst_status(1);$
