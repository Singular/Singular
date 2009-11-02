LIB "tst.lib";
LIB "classify.lib";

tst_init();
tst_ignore("CVS: $Id$");


"======================================================================";
"// running tetst for corank<=2";
ring r=0,(x,y),ds;
poly f;
map phi=r,y-x^2,x+y;

  f=x^4+y2; // A[3]
  classify(phi(f));

"======================================================================";
"// Step 3";
  f=x2y+y3; // D[6]
  classify(phi(f)); // D[6] // D[4]

  f=x2y+y5; // D[6]
  classify(phi(f));

"======================================================================";
"// Step 6-12";
  f=x3+xy7+y10; // E[18]
  classify(phi(f));

  f=x3+xy5+y8; // E[13]
  classify(phi(f));

  f=x3+xy10+y14; // E[26]
  classify(phi(f));

  f=x3+x2y3+xy7+y9; // J[3,0]
  classify(phi(f));

  f=x3+x2y2+y11; // J[2,5]
  classify(phi(f));

"======================================================================";
"// Step 13";
  f=x4+x3y+x2y2+xy3; // X[1,0]
  classify(f);

  f=x4+y4; // T[4,4,2]
  classify(phi(f));

  f=x4+x3y+x2y2+y6; // X[1,2]
  classify(phi(f));

//f=x4+2x3y+x2y2+2x2y4+2xy5+y6+y8; // Y[1,2,2]
"// The next step returns an error message. Y[1,p,q] cannot be recognized";
  f=x4+2x3y+x2y2+x2y4+x2y7+2xy8+y9+y11; // Y[1,5,2]
  classify(phi(f));

"======================================================================";
"// Step 17-24";
  f=x3y+xy6+y8; // Z[17]
  classify(phi(f));

  f=x3y+xy8+y12; // Z[24]
  classify(phi(f));

  f=x3y+xy7+y9; // Z[19]
  classify(phi(f));

  f=x3y+x2y5+xy10+y13; // Z[3,0]
  classify(phi(f));

  f=x3y+x2y9+y29; // Z[7,4]
  classify(phi(f));

"======================================================================";
"// Step 25-";
f=x4+x2y3+xy4+y5; // W[24], step 27
classify(phi(f));

f=x4+x2y5+xy7+y10; // W[25], step 28
classify(phi(f));

f=x4+x2y7+xy11+y14; // W[3,0], step 30
classify(phi(f));

f=x4+x3y5+x2y9+y26; // W[4,8], step 31
classify(f);

f=x4+2x2y5+x2y7+y10+xy10; // W#[2,4], step 32
classify(phi(f));

f=x4+2x2y6+x2y7+y11+xy8; // W[29], step 34
classify(phi(f));

f=x4+x2y8+xy12+y15; // W[42], step 35
classify(phi(f));

f=x4+x3y2+x2y4+xy6; // X[2,0], step 37
classify(phi(f));

f=x4+x3y4+x2y8+y18; // X[4,2], step 38
classify(phi(f));

f=x4+2x3y4+x2y8+x2y10+x2y13+2xy14+y18+y23; // Y[4,2,5], step 39
classify(phi(f));

f=x4+x3y2+x2y7+xy9+xy10+y12; // Z[2,29], step 42, k=2, i=1
phi=r,y-x,y+x;
classify(f);

f=x4+x3y2+x2y9+xy11+xy14+y16; // Z[2,36], step 43, k=2, i=2
classify(f);

f=(x+3y3)*(x3+xy18+y26); // Z[3,67], step 44, 	k=3, i=5
classify(f);

f=(x+3y4)*(x3+x2y6+xy13+y18);  // Z[4,2,0], step 45, k=4, i=2
classify(f);

f=x4+x3y2+x2y7+x2y11+xy13+y18; // Z[2,3,1], step 46, k=2, i=3, p=1
classify(phi(f));


kill r;

////////////////////////////////////////////////////////////////////////////////
// Corang 3
//
"======================================================================";
"// running tetst for corank=3";
ring r=0,(x,y,z),ds;
poly f;
map phi=r,y-x^2,x+y,y-z;

//f= ; // T[3,3,3], step 51
//f= ; // T[3,3,], step 53
//f= ; // T[3,,], step 55
//f= ; // T[,,], step 57

//f= ; // Q[], step 60
//f= ; // Q[], step 61
//f= ; // Q[], step 62
//f= ; // Q[,0], step 64
//f= ; // Q[,], step 65

"======================================================================";
"// steps 62-81 are not implemented => not tests";

//f= ; // U[], step 84
//f= ; // U[,0], step 86
//f= ; // U[,], step 87
//f= ; // U[], step 89

"======================================================================";
"// steps 91-96, fix bug in algorythme before implementation";

//f= ; // V[1,0], step 99
//f= ; // V[1,], step 100
//f= ; // V#[1,0], step 101
//f= ; // V#[1,], step 102

//classify(f);

"=====================================================================";
example classify;
"======================================================================";
"// reomving database.";
int i=system("sh", "/bin/rm -f NFlist.dir NFlist.pag");
tst_status(1);$
