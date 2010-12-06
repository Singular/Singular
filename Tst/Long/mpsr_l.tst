LIB "tst.lib";
tst_init();
LIB "general.lib";

option(noredefine);

proc vcheckdump(link l, string vn)
{
  if (typeof(`vn`) != "proc" && typeof(`vn`) != "link")
  {
    write(l, `vn`);
    read(l);
    if (typeof(`vn`) == "ring")
    {
       setring `vn`;
       rcheckdump(l, names(`vn`));
    }
  }
}
  
proc rcheckdump(link l, list nameslist)
{
  int i;
  for (i=1; i<size(nameslist); i++)
  {
    vcheckdump(l, nameslist[i]);
  }
}

proc checkdump(link l)
{
  rcheckdump(l, names());
}

// data
int i;
int i1 = 1;
int i2 = -100092;

intvec iv;
intvec iv1 = 1,2,3;
intvec iv2 = -1,2,-3,4,-5;

intmat im;
intmat im1[2][3]=1,3,5,7,8;
intmat im2[3][3]= -1,2,-3,4,-5;

string s;
string s1 = "Hello World";

list l;
list l1 = i,iv,im,i1,iv1,im1;
list l2 = l1, iv2, l, im2;

ring r;

poly p;
number n;
number n1 = 2;
number n2 = -7;

poly p1 = x + y;
poly p2 = xyz - 2x3y4z5 + 3xy -4yz + 5z;
poly p3 = p2^4;

vector v;
vector v1 = [p1, p2];
vector v2 = [p, p1,p2, p3] + p1*gen(5);

ideal id;
ideal id1 = p, p1, p2, p3;
ideal id2 = p, p1+p2, p2+3, p1+p2+p3, p1, p2, p3;

module mv;
module mv1 = v, v1, v2;
module mv2 = v1+v2, p1*v2, p2*v1, v, v1, v2;

matrix m;
matrix m1[2][3] = p, p1, p2, id1;
matrix m2[4][4] =  p, p1, p2, p3, id1, id2;


ideal j = std(id1);

// the following does not work properly -- run it with Singularg and
// you'll see what I mean -- debug it eventually !!!
// qring q = j;
poly p11 = x + y;
poly p12 = xyz - 2x3y4z5 + 3xy -4yz + 5z;
poly p13 = p12^4;

ring rtm = 32003, (x,y,z,t), dp;
ideal ex = t3y7z4 + 9x8 + 5t2x4y2 + 2t2xy2z3,
2t2x5y4+7txy6+9y8 + 2t2x2yz3,  3t4x2y3z2 + 9z8, 3x2y9 +
y9 + 5x4;

ring rt0 = 0, (x,y,z,t), dp;
ideal ex = t3y7z4 + 9x8 + 5t2x4y2 + 2t2xy2z3,
2t2x5y4+7txy6+9y8 + 2t2x2yz3,  3t4x2y3z2 + 9z8, 3x2y9 +
y9 + 5x4;


ring rr = 32003,(a,b),dp;
map f = r, a,b,a+b;
map g   = rr,a2,b2;
map phi = g(f);


ring r0 = 0, x, lp;
number n;
number n1 = 29734481274863241234589;
number n2 = n1/(n1-6);

poly p;
poly p1 = n1*x + n2*x^4 + 7/3*x^2*x^5 - 6;
poly p2 = x*x*x - 2*x^3*x^4*x^5 + 3*x*x -4*x*x + 5*x^2 + p1;
poly p3 = p2*p2;

vector v;
vector v1 = [p1, p2];
vector v2 = [p, p1,p2, p3] + p1*gen(5);

ideal id;
ideal id1 = p, p1, p2, p3;
ideal id2 = p, p1+p2, p2+3, p1+p2+p3, p1, p2, p3;

module mv;
module mv1 = v, v1, v2;
module mv2 = v1+v2, p1*v2, p2*v1, v, v1, v2;

matrix m;
matrix m1[2][3] = p, p1, p2, id1;
matrix m2[4][4] =  p, p1, p2, p3, id1, id2;


ring r1 = 0, x(1..10), ls;

number n;
number n1 = 29734481274863241234589;
number n2 = n1/(n1-6);

poly p;
poly p1 = n1*x(1) + n2*x(5)^4 + 7/3*x(3)^2*x(4)^5 - 6;
poly p2 = x(2)*x(5)*x(6) - 2*x(7)^3*x(8)^4*x(9)^5 + 3*x(1)*x(5) -4*x(5)*x(9) +
5*x(9)^2 + p1;
poly p3 = p2*p2;

vector v;
vector v1 = [p1, p2];
vector v2 = [p, p1,p2, p3] + p1*gen(5);

ideal id;
ideal id1 = p, p1, p2, p3;
ideal id2 = p, p1+p2, p2+3, p1+p2+p3, p1, p2, p3;

module mv;
module mv1 = v, v1, v2;
module mv2 = v1+v2, p1*v1, v1-v2, v, v1, v2;

matrix m;
matrix m1[2][3] = p, p1, p2, id1;
matrix m2[4][4] =  p, p1, p2, p3, id1, id2;

ring r2=10,(x(1..6)),(lp(2),dp(4));
number n;
number n1 = 29734481274863241234589;
number n2 = n1/(n1-6);

poly p;
poly p1 = n1*x(1) + n2*x(5)^4 + 7/3*x(3)^2*x(4)^5 - 6;
poly p2 = x(2)*x(5)*x(6) - 2*x(1)^3*x(2)^4*x(3)^5 + 3*x(1)*x(5) -4*x(5)*x(3) +
5*x(3)^2 + p1;
poly p3 = p2*p2;

vector v;
vector v1 = [p1, p2];
vector v2 = [p, p1,p2, p3] + p1*gen(5);

ideal id;
ideal id1 = p, p1, p2, p3;
ideal id2 = p, p1+p2, p2+3, p1+p2+p3, p1, p2, p3;

module mv;
module mv1 = v, v1, v2;
module mv2 = v1+v2, p1*v2, v1-v2, v, v1, v2;

matrix m;
matrix m1[2][3] = p, p1, p2, id1;
matrix m2[4][4] =  p, p1, p2, p3, id1, id2;

ring r3=(7,a, b, c),(x,y,z),dp;
number n;
number n1 = 2*a^2*b*c -3*b*c + -1*a + 4;
number n2 = n1 / (n1 - 5c)^2;

poly p;
poly p1 = x + y + a;
poly p2 = 3*a^2*xyz*n1 - 2*n2*x3y4z5 + 3xy -4yz + n1*z;
poly p3 = p2*p1;

vector v;
vector v1 = [p1, p2];
vector v2 = [p, p1,p2, p3] + p1*gen(5);

ideal id;
ideal id1 = p, p1, p2, p3;
ideal id2 = p, p1+p2, p2+3, p1+p2+p3, p1, p2, p3;

module mv;
module mv1 = v, v1, v2;
module mv2 = v1+v2, p1*v2, v1-v2, v, v1, v2;

matrix m;
matrix m1[2][3] = p, p1, p2, id1;
matrix m2[4][4] =  p, p1, p2, p3, id1, id2;

ring r4=(7,a),(x,y,z),dp;
minpoly=a^2+a+3;
poly p;
number n;
number n1 = 2*a^2 + 3;
number n2 = n1 / (n1 -5)^3;

poly p1 = x + y + a;
poly p2 = 3*a^2*xyz*n1 - 2*n2*x3y4z5 + 3xy -4yz + n1*z;
poly p3 = p2^4;

vector v;
vector v1 = [p1, p2];
vector v2 = [p, p1,p2, p3] + p1*gen(5);

ideal id;
ideal id1 = p, p1, p2, p3;
ideal id2 = p, p1+p2, p2+3, p1+p2+p3, p1, p2, p3;

module mv;
module mv1 = v, v1, v2;
module mv2 = v1+v2, p1*v2, v1-v2, v, v1, v2;

matrix m;
matrix m1[2][3] = p, p1, p2, id1;
matrix m2[4][4] =  p, p1, p2, p3, id1, id2;

ring r5=(49,a),(x,y,z),dp;
poly p;
number n;
number n1 = 2*a^2 + 3;
number n2 = n1 / (n1 -5)^3;

poly p1 = x + y + a;
poly p2 = 3*a^2*xyz*n1 - 2*n2*x3y4z5 + 3xy -4yz + n1*z;
poly p3 = p2^4;

vector v;
vector v1 = [p1, p2];
vector v2 = [p, p1,p2, p3] + p1*gen(5);

ideal id;
ideal id1 = p, p1, p2, p3;
ideal id2 = p, p1+p2, p2+3, p1+p2+p3, p1, p2, p3;

module mv;
module mv1 = v, v1, v2;
module mv2 = v1+v2, p1*v2, v1-v2, v, v1, v2;

matrix m;
matrix m1[2][3] = p, p1, p2, id1;
matrix m2[4][4] =  p, p1, p2, p3, id1, id2;

ring r6 = 32003, (x,y,z), wp(1,2,3);
poly p = (xyz - 2x3y4z5 + 3xy -4yz + 5z + 2)^2;

ring r7 = 32003, (x,y,z), Ws(1,2,3);
poly p = (xyz - 2x3y4z5 + 3xy -4yz + 5z + 2)^2;

ring r8 = 32003, (a,b,c,x,y,z), (dp(3), wp(1,2,3));
poly p = (xyzabc - 2x3y4z5a2b4c3 + 3xya2 -4yzb3c2 + 5zc + 2)^2;

ring r9 = 32003, (a,b,c,x,y,z), (a(1,2,3,4,5),Dp(3), ds(3));
poly p = (xyzabc - 2x3y4z5a2b4c3 + 3xya2 -4yzb3c2 + 5zc + 2)^2;

ring r10 = 32003, (x,y,z), M(1, 0, 0, 0, 1, 0, 0, 0, 1);
poly p = (xyz - 2x3y4z5 + 3xy -4yz + 5z + 2)^2;

ring r11 = 0, x(1..10), (lp(2), M(1, 2, 3, 1, 1, 1, 1, 0, 0), ds(2), ws(1,2,3));
poly p1 = x(1) + 123399456085/(123399456085 + 1)*x(5)^4 + 7/3*x(3)^2*x(4)^5 - 6;
poly p2 = x(2)*x(5)*x(6) - 2*x(7)^3*x(8)^4*x(9)^5 + 3*x(1)*x(5) -4*x(5)*x(9) +
5*x(9)^2 + p1;

ring r12 = 0, (x,y,z), (C,lp);
vector v1 = [x+y2,z3+xy];
vector v2 = [x,x,x];

ring r13 = 0, (x,y,z), (lp,c);
vector v1 = [x+y2,z3+xy];
vector v2 = [x,x,x];

ring r14 = 0, (x,y,z), (c,lp);
vector v1 = [x+y2,z3+xy];
vector v2 = [x,x,x];

// First, make ASCII-dump of everything 
dump(":w _dump.txt");
killall();
getdump("_dump.txt");

// Does our version have MP?
if (! (system("with", "MP")))
{
  // no -- so simply dump everything to stdout and exit
  dump("");
  $;
}

// Yes, so let's make an MPfile dump
dump("MPfile: _dump.mp");
killall();
getdump("MPfile:r _dump.mp");

// checkdump("MPfile: _dump.mp");

// MPfork dump
link ll = "MPtcp:fork"; 
open(ll);
if (status(ll, "openwrite", "yes"))
{
  write(ll, quote(getdump(link_ll))); dump(ll); read(ll);
  killall("not", "link");
  write(ll, quote(dump(link_ll))); getdump(ll); read(ll); 

  checkdump(ll);
}
kill ll;


// MPlaunch dump
string sing = system("Singular");

if (size(sing))
{
  // check whether rsh works and whether remote Singular can be executed
  if (system("sh","rsh `hostname` -n test -x "+sing+" 1>/dev/null 2>&1"))
  { 
    if (system("sh","remsh `hostname` -n test -x "+sing+" 1>/dev/null 2>&1"))
    {
      if (system("sh","ssh `hostname` -f test -x "+sing+" 1>/dev/null 2>&1"))
      {
        sing = "";
      }
    }
  }
  
  if (size(sing))
  {
    link ll = "MPtcp:launch"; 
    open(ll);
    if (status(ll, "openwrite", "yes"))
    {
      kill sing;
      write(ll, quote(getdump(link_ll))); dump(ll); read(ll);
      killall("not", "link");
      write(ll, quote(dump(link_ll))); getdump(ll); read(ll); 

      checkdump(ll);
    }
    close(ll);
  }
}

if (defined(sing))
{
  kill sing;
}
tst_ignore(system("sh", "rm -rf _dump.*"));

killall("proc");
dump("");
tst_status(1);$


  

