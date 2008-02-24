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
poly p3 = p2^2;

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


// First, make two (preserves order of variables) ASCII-dumps of everything
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
  write(ll, quote(getdump(mp_ll))); dump(ll); read(ll);
  killall("not", "link");
  write(ll, quote(dump(mp_ll))); getdump(ll); read(ll);

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
      write(ll, quote(getdump(mp_ll))); dump(ll); read(ll);
      killall("not", "link");
      write(ll, quote(dump(mp_ll))); getdump(ll); read(ll);

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
LIB "tst.lib";
tst_status(1);$
