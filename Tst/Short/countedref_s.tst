// File: countedref_s.tst
// Tests for reference and shared data types
LIB "tst.lib";
tst_init();

system("reference");
system("shared");




// reference huhu=17;
int koko=17;

reference huhu=koko;
// string(huhu);
//string(huhu);
int(huhu);
//string(huhu);


huhu;

string(huhu);
reference hihi = huhu;

hihi;

hihi;
string(hihi);

ring r=0,x,dp;

number(hihi);
int(hihi);

hihi+huhu;
hihi+huhu;
hihi*hihi;

hihi*12;
huhu+10;
hihi+10;



int obj =19;
reference pobj = obj;
//attrib(pobj, "huhu","qweqw");

//ython_run("def myprint(*args): print args");
//attrib(myprint);
//attrib(myprint, "func_name");

kill obj, pobj;

//attrib(myprint, "func_name", "byAnyOtherName");
//attrib(myprint, "func_name");

//list(huhu,huhu,hihi);

"qwewqewqeqw";

huhu;
hihi;
"da";
huhu="qweeqw";
"hier";
huhu;
hihi;

typeof(huhu);

int neunzehn = 19;
reference ref1 =neunzehn;
reference ref2 = ref1;
def ref3=ref1;

(ref1,ref2,ref3);
//ref1=16;

(ref1,ref2,ref3);
list lllll = list(17,13);
reference ref1 = lllll;
ref2 = ref1;
ref3=ref1;
"huhu";


ref1;ref2;ref3;

//ref2[1]=9;
ref1;ref2;ref3;
ref_extract(ref2);
ref_extract(ref1);


list lll = list(1,2);
reference lref = lll;

lref;
"lref[2]:";

int elt = lref[2];
elt;
lref[2]=3;

lref;
//Countedref::setitem(lref,2,3);
lref;
//lref[2];
"skip write";///lref[2]=8;
//lref[2];
"huhu";
lref;
"hihi";
list ll=(1,2,3);
ll[2];
ll[2]=7;
ll;



reference part = ll[2];
part;
part=17;
ll;

typeof(part);


ring r =0,(x,y,z),lp;

poly p = x+y+z;

reference pref = p;

pref;

ring q = 0,(x,y),lp;

pref;


setring(r);
pref;
poly ppp = 7;
reference pppref = ppp;

setring(q);
poly pp=1;

reference ppref = pp;


pref + ppref;

pref + pppref;

setring(r);
pref + pppref;
kill pppref;
ring r=0,(x,y,z),dp;
poly f=x2+y2+z2+x+y+z;

poly xp=x;
poly yp=y;
poly zp=z;
reference fref = f;
reference xref=xp;
reference yref=yp;
reference zref=zp;
subst(f,x,link(yref),y,zp);
subst(fref,x,yp,y,zp);
subst(fref,x,yp,y,zref);

ring r=0,(x,y,z),dp;
poly p = x+y;
reference ref7=p;
ref7;
ring q=0,(x,y),dp;
ref7;
poly p = x+y;
ref7=p;
ref7;

newstruct("mystruct", "int i, int j");

mystruct stru;
stru.i = 17;

//stru.j =19;
stru.i;
//stru;

//reference sref = stru;

//sref;
//sref.i=9;
//sref;
//stru;

reference spart = stru.i;
//spart;
//spart;
//spart;
spart =12;
spart;
stru;



list listoflists = list(list(1,2,3));

reference lolref = listoflists;

lolref[1];

lolref[1][2];
lolref[1][2]=9;
listoflists;
mystruct ms;

ms.i=2;
ms.j=3;
list listofnewstr = list(ms);



reference lonren = listofnewstr;

lonren[1];
listofnewstr[1].i;// doesn'nt work

reference refelt = listofnewstr[1];
refelt.i = 7;

lonren;
listofnewstr;

reference refelt2 =lonren[1];
refelt2.i = 17;
listofnewstr;


//reference rseq=(1,2,3);

//rseq;


def copyref = refelt2;

copyref;
typeof(copyref);

list ll17=list(1,2,4);
shared sh17 = list(1,2,4);

sh17;
sh17[1]=7;

sh17;

reference ref17 = sh17;

ref17[2]=19;
sh17;

shared ll19=ref17;
ll19[3]=9;

sh17;


 typeof(link(sh17));

shared sh19 = list(7,8,9);

sh19;

sh19[2]=5;
sh19;


list ll23;
reference ref23 =ll23;
ref23;


kill ll23;
ref23;

list ll24 = list(1,2,3);
shared data = ll24[2];
shared dataA = data;
data;
typeof(data);
data =17;
typeof(data);

data;
dataA;


typeof(link(data));

//  list ll24 = list(1,2,3);
ll24;

shared data2 = ll24;
typeof(link(data2));
data2;
data2[2];
data2[2] =19;
data2[2];
data2;


shared data3 =12;
data3;
data3 =13;
data3;



newstruct("teststruct","int i,int j");

teststruct ts;
reference tsref =ts;

tsref.i=17;
ts;


shared tssh=ts;

tssh.j = 9;

ts;
tssh;

int ival=17;
shared iref =ival;
shared iref2 =iref;

iref2 = 19;

iref;
iref2;
ival;


list lval=list(56,9);
shared lRef =lval;
shared lref2 =lRef;

lref2 = list(324,253);

lRef;
lref2;
lval;

// some debugging commands

int db; int db2;
reference dbref = db;
reference dbref2 = dbref;
reference dbref3 = db;

system(dbref, count);
system(dbref, same, dbref);
system(dbref, same, dbref2);
system(dbref, same, dbref3);
system(dbref, same, db2);

system(dbref, likewise, dbref);
system(dbref, likewise, dbref2);
system(dbref, likewise, dbref3);
system(dbref, like, dbref);
system(dbref, like, dbref2);
system(dbref, like, dbref3);
system(dbref, like, db);
system(dbref, like, db2);

system(dbref, "typeof");
system(dbref, "type");

system(dbref, name);
system(dbref2, name);

system(dbref, enumerate) == system(dbref2, enumerate);
system(dbref, enumerate) == system(dbref, enumerate);
system(dbref, enumerate) != system(dbref3, enumerate);


shared mem = 17;
shared mem2 = mem;
shared mem3 = link(mem);

system(mem, count);
system(mem, same, mem);
system(mem, same, mem2);
system(mem, same, mem3);
system(mem, same, db2);

system(mem, likewise, mem);
system(mem, likewise, mem2);
system(mem, likewise, mem3);
system(mem, like, mem);
system(mem, like, mem2);
system(mem, like, mem3);
system(mem, like, db);
system(mem, like, db2);

system(mem, "typeof");
system(mem, "type");

system(mem, name) == system(mem2, name);

system(mem, enumerate) == system(mem2, enumerate);
system(mem, enumerate) == system(mem, enumerate);
system(mem, enumerate) != system(mem3, enumerate);

reference memref = mem;
system(mem, same, memref);
system(mem, likewise, memref);

// subexpressions
shared shll =19;
list llsh;
llsh [1] = shll;
typeof(llsh[1]);
llsh;

shared shll2 =12;

llsh[1] = shll2;

llsh;

shll;

shared shll3 =llsh[1];
shll3 =14;


llsh;

shll;

reference llshref = llsh[1];
llshref = 13;
llsh;
shll;

typeof(llsh[1]);


shared sh5=5;

list ll5=list(sh5);

ll5[2]= sh5;
ll5[2]=7;
ll5;
sh5;


def defed = sh5;
typeof(defed);
defed=17;

sh5;

shared  shll6=list();

shared shll7=shll6;
shll6[1]=17;

shll7;


newstruct("mystrsh", "int i");

mystrsh mystrval8;

shared val8 = mystrval8;
val8;

shared val9= val8;
val8.i=17;

val9;

newstruct("mystrsh2", "def sh");

mystrsh2 mystrval10;

shared val17 = 17;
mystrval10.sh = val17;
mystrsh2 mystrval11 = mystrval10;

mystrval10;
mystrval11;

shared ref10sh= mystrval10.sh;
ref10sh=10;

mystrval11;
system(mystrval11.sh, name)==system(mystrval10.sh, name);
shared val11 =  mystrval11.sh;
val11=9;
mystrval11;
mystrval10;


shared(mystrval11.sh) = 11;



mystrval10;
system(mystrval11.sh, name)==system(mystrval10.sh, name);;

shared myllsh= 19;
list myllval;
myllval[1]= myllsh;

shared(myllval[1]) = 17;
myllval;
myllsh;

shared(myllval[1]) = 18;
myllval;
myllsh;

shared shvalOFmyllval_1 = myllval[1];
shvalOFmyllval_1 = 3;
myllsh;
typeof(myllval[1]);



proc replace(def lhs, def rhs) { lhs = rhs; }

replace(myllval[1], 14);

myllval;
myllsh;

shared(myllval[1]) =15;
typeof(myllval[1]);

myllval;
myllsh;

myllval[1] =16;

typeof(myllval[1]);
myllval;
myllsh;

link(myllsh);


shared val=1;
val;
list ll=list(val);
ll;
link(ll[1])=8;
val;
ll;
shared vv =ll[1];
vv=9;
ll;
vv;
val;
reference vvv=ll[1];
vvv=9;
ll;
typeof(ll[1]);
ll[1]=val;
shared vv =ll[1];





ring r2=0,x,dp;

poly p = x+1;

shared p_sh =p;


ring r3=0,x,dp;

p_sh;

setring(r2);

p_sh;



kill mem, mem2, mem3;
shared mem = list(17);
shared mem2 = mem;
shared mem3 = link(mem);

system(mem, count);
system(mem, same, mem);
system(mem, same, mem2);
system(mem, same, mem3);
system(mem, same, db2);
system(mem, like, mem);
system(mem, like, mem2);
system(mem, like, mem3);
system(mem, like, db2);


shared llr = list(1,2,3);

llr;
ring r=0,x,dp;

llr[1]=x+1;

llr;

ring q=0,y,dp;

llr;

setring(r);

llr;


shared sharedp =x;
sharedp;
setring q;
sharedp;
setring(r);
sharedp;
setring q;
sharedp=y;
sharedp;

setring(r);
sharedp;
setring q;
sharedp;

setring(r);
poly xpoly=x;
reference refp =xpoly;
refp;
setring q;
refp;
setring(r);
refp;
setring q;
refp=y;
refp;

setring(r);
refp;
setring q;
refp;



kill ll, r, q;

shared ll =list(1,2,3);
 ring r=0,x,dp;
ll[1] =x;
ring q=0,y,dp;
ll;
ll[1];
ll;
setring r;
ll;
setring q;
ll[1]=y;
ll;
setring r;
ll;
ll[1];
setring q;
ll[1];

"qewqe";

"qweewq";

shared llshort=list(1,2);

llshort;
llshort[3]=17;

llshort;

/// Pratical examples: cache
echo=1;
newstruct("sqcache", "shared table");

proc cached_square(sqcache cache, int arg)
{
  if (arg == 0) { return (0); }
  shared value = cache.table[arg];
  if (system(value, undef)) { value = arg^2; }
  return (int(value));
}

system("install", "sqcache", "(", cached_square, 4);
sqcache square;

shared init = list();
square.table = init;

int t=timer;
int i, j,k,n;
n = 100;
init=(0:n);
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }
  for( i = 0; i <n; i++) {
   k = square(i);
  }



int time1 = timer -t;
t=timer;
list cll=(0:n+1);
  for( i = 1; i <=n; i++) {
   cll[i] = (i^2);
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }
  for( i = 1; i <=n; i++) {
   k = cll[i];
  }





int time2 = timer -t;




//time1;
//time2;


newstruct("bdd", "int idx,shared high,shared low");
bdd diag;
shared zero=0;
diag.high=zero;
diag.low=zero;
diag.idx = 0;

//for (i=0; i <=n; i++)
n=20;
proc chain(shared arg)
{
bdd diag;
diag.idx = arg.idx+1;
diag.high=arg;
diag.low=arg;
return (diag);
}


t=timer;
for (i=0; i<=n; i++ )
{
  diag = chain(diag);
}
//timer-t;

shared iter=diag;
//while(int(iter.idx)) {
//  iter.idx;
//  iter = iter.high;
//}

proc print_bdd(shared iter) {
  string res = "("+string(iter.idx)+", ";
  shared it1=iter.high;
  shared it0=iter.low;

 if (int(iter.idx))
 {
  res=res +  print_bdd(it1) + " : ";
  res= res+ print_bdd(it0)+ ")";
 }
 return (res);// print_bdd(iter.low);
}
shared iter2=diag;

//print_bdd(iter2);

newstruct("bdd0", "int idx,def high,def low");
bdd0 diag0;
diag0.idx=0;
diag0.high=0;
diag0.low=0;


proc chain0(bdd0 arg)
{
bdd0 diag;
diag.idx = arg.idx+1;
diag.high=arg;
diag.low=arg;
return (diag);
}

/// Deactivate "classical" way, take too long (many copies)
n=0;
t=timer;
def val;
for (i=0; i<=n; i++ )
{
  val =chain0(diag0);
   diag0=val;
}
//timer-t;

proc print_bdd0(def iter) {
  string res = "("+string(iter.idx)+", ";
  def it1=iter.high;
  def it0=iter.low;

 if (int(iter.idx))
 {
  res=res +  print_bdd0(it1) + " : ";
  res= res+ print_bdd0(it0)+ ")";
 }
 return (res);// print_bdd(iter.low);
}


//print_bdd0(diag0);

//diag;

kill sh17,sh18;
bdd b01;

(b01.idx)=0;
shared sh17=17;
shared sh18=18;
b01.high = sh17;
b01.low  = sh18;


b01.high;
b01.low;

def(b01.high) = 19;
def(b01.low)  = 20;

b01.high;
b01.low;

sh17;
sh18;
kill ll;

shared ll1=1; shared ll2=2;
list ll=list(ll1, ll2);
typeof(ll[1]);


def(ll[1]) = 3;
ll;
ll1;
typeof(ll[1]);


shared(ll[1]) = 5;
ll;
ll1;
typeof(ll[1]);


shared ll3=2;

ll[3]=ll3;

typeof(ll[3]);
ll;

def(ll[3])=4;

ll3; ll;
typeof(ll[3]);

ll[4]=1;

ll[4] = ll3;
ll;
def(ll[4])=17;
ll;


bdd b02;

b02.high;
def(b02.high);
def(b02.high)=9;

b02.high;
typeof(b02.high);

kill ll;

list ll;
ll[1] = ll1;
shared(ll[1]) = 3333;
ll;



kill ll;

shared ll=list();
shared llsync=ll;
def(ll[1])=list(3,4);
ll;

def(def(ll[1])[3])=list(5,6);
ll;

def(ll[1])[3]=list(8);
ll;


ll[1][3]=list(6);
ll;
llsync;

kill sh;
newstruct("bbx", "def i");
bbx box;

shared sh=box;
sh;
sh.i =list(1,2);
sh;

sh.i[2]=19;

sh;
sh.i;

system(sh, "help");


kill i, iref;
int i =9;
shared iref = i;
link l1 = "ssi:fork"; open(l1);
write(l1, quote(list(iref, iref)));

waitfirst(list(l1), 500);
def argi = read(l1);
argi;
typeof(argi[1]);
typeof(argi[2]);


//typeof(link(argi));
close(l1);


kill i, iref,l1;
int i =9;
reference iref = i;
link l1 = "ssi:fork"; open(l1);
write(l1, quote(def(iref)));

waitfirst(list(l1), 500);
def argi = read(l1);
argi;
typeof(argi);
typeof(link(argi));
close(l1);

i=10;
argi;

shared backtest=list(17,9);

backtest;

shared backtest_1 = backtest[1];

backtest_1;

backtest_1=6;
backtest;
kill backtest;

backtest_1;

backtest_1;

kill i, ll;
int i = 17;
reference ref = i;

ref;
ref + ref;  // Automated 'dereferencing'
ref + 4;
4 + ref;


list ll=(1,2,3,4,5);
ref=3;
ll[ref];

kill r;
ring r=0,(x,y,z),dp;
poly p = x+y+z;
shared xsh = x+1;
poly(xsh);
poly(xsh);
poly(xsh);



// Checking side effects
list changeme;
changeme;

proc setfirst(reference ll, def arg) { ll[1] = arg; }

setfirst(changeme, 17);
changeme;

// Checking differenct variants of list conversion

shared shlist = list(1,2,3);
list thelist = shlist;
thelist;
list(shlist);
link(shlist);

/// Checking that link deeply copies indeed
kill r, xsh, ll;
ring r = 0, (x,y,z), dp;
poly p = x + y + z;
shared xsh = x;

subst(p, xsh,1, y,2, z,3);        // fails
subst(p, poly(xsh),1, y,2, z,3);  // good
subst(p, link(xsh),1, y,2, z,3);  // fine
subst(p, link(xsh),1, y,2, z,3);  // fine
subst(p, link(xsh),1, y,2, z,3);  // fine

list ll = list(xsh, xsh, xsh);
ll[1] = y;      // replaced only first entry
ll;
shared(ll[2]) = z;    // replaces the others
ll;
def(ll[2]) = x;       // generic alternative
ll;

// --------------------------------------------------------
kill r, ref, ref2;
ring r;ideal I = maxideal(1);ideal J = I;
reference ref = I;

attrib(ref, "huhu", 11);

attrib(ref, "huhu");
attrib(I, "huhu");

attrib(ref, "huhu", 111);

attrib(ref, "huhu");
attrib(I, "huhu");


reference ref2= I;
attrib(ref2, "huhu");

attrib(ref, "hihi", 12);

attrib(ref, "huhu");
attrib(I, "huhu");
attrib(ref, "hihi");
attrib(I, "hihi");

attrib(ref2, "huhu");
attrib(ref2, "hihi");

attrib(I, "isSB");
attrib(ref2, "isSB");

attrib(ref2, "isSB", 0);

attrib(I, "isSB");


attrib(ref2, "isSB");



attrib(I, "isSB", 1);
attrib(I, "isSB");

attrib(ref2, "isSB");

shared shval= I;
// Ensure reloading does not damage something
system("reference");
system("shared");
reference refTwo = ref2;
shared shval2 = shval;
refTwo;
shval2[1] == shval[1];
kill refTwo, shval2;
system("reference");
system("shared");
reference refTwo = ref2;
shared shval2 = shval;
refTwo;
shval2[1] == shval[1];
kill refTwo, shval2;

// --------------------------------------------------------
tst_status(1);$

