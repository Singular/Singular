LIB "tst.lib";
tst_init();

// Assignment of string literals and integers
pyobject pystr = "a string literal";
pyobject pyone = 1;


pystr;
pyone;


// Checking python errors
pystr + pyone;
pystr();
pystr(1, pyone);

// Compute something in python
def result = python_eval("4+5");
result;

int(result);

// Execute python commands from Singular
python_run("def myprint(*args): print args");

myprint("Seventeen: ", 17);
myprint();

// Get type
typeof(myprint);

// Get String
string(python_eval("'pystring'"));

// Get attribute
attrib(myprint, "func_name");

// Check initialization
pyobject empty;
empty;
typeof(empty);


// python list from Singular list and access item
pyobject ll = list(1, 2, 3);
ll;
ll[1];
proc(attrib(ll,"__getitem__"))(2);


//// Works, is PolyBoRi is also delivered
// python_import("polybori");
// declare_ring(list(Block("x", 10), Block("y", 10)));
// list polybori_ideal = (x(1)+x(2),x(2)+y(1));
// def result1 = groebner_basis(polybori_ideal);
// result1;
// typeof(result1);
// result1[1];


// Check whether already defined objects will be overwritten
def already_defined = list(1);
already_defined;

python_run("def already_defined(): return 17");
already_defined();

def already_defined = "overwritten";
already_defined;

python_run("defined_by_python = range(10)");

defined_by_python;
python_run("del defined_by_python");

// Should rise error message now:
defined_by_python;

// Test python import
python_import("os");
name;
sep;
linesep;

// Numerical operations
pyobject two = 2;
pyobject three = 3;

two + three;
two - three;
two * three;
two / three;
two ^ three;
two ** three;

three < two;
two < three;
three <= two;
two <= three;
two == three;
two == two;
three > two;
two > three;
three >= two;
two >= three;
two != two;
two != three;

pyobject pystr = "Hello";
pystr + " World!";
pystr * 3;

// Test attrib command
attrib(myprint);
attrib(myprint, "func_name");
attrib(myprint, "func_name", "byAnyOtherName");
attrib(myprint, "func_name");

pyobject pystr2 = "pypypypyp";
proc(attrib(pystr2, "count"))("y");
pystr2::"count"("p"); // simplified notation
pystr2::count("y");   // even more simplified
pystr2.count("y");    // simplest

// testing member function call
python_run("def new_pyobj(): pass");
attrib(new_pyobj, "new_attr", "something");
attrib(new_pyobj, "new_attr");
attrib(new_pyobj);

killattrib(new_pyobj, "new_attr");
attrib(new_pyobj);

killattrib(new_pyobj, "new_attr"); // -> error message
killattrib(new_pyobj, 17);         // -> error message

attrib(new_pyobj, "new_attr");     // -> error message

// python list from empty Singular list (bug fixed?)
pyobject ll_empty = list();
ll_empty;

// converting to and from intvec
intvec v0;
v0;
pyobject obj0 = v0;
obj0;

intvec v=(1,2,3);
pyobject obj = v;
obj;
intvec(obj);

intvec(python_eval("[1,2,9999999999]")); // -> error
intvec(python_eval("[1,2,list()]"));     // -> error

ring r=0,x,lp;
def rl = ringlist(r);

python_eval("tuple")(list(rl[1..3]));

// interaction with newstruct'ed type
newstruct("wrapping","pyobject p");
wrapping wrapped;

pyobject seventeen = 17;
wrapped.p = seventeen;
wrapped;

proc unwrap(wrapping arg) { return (arg.p); }
system("install", "wrapping", "pyobject", unwrap, 1);

pyobject released = wrapped;
released;

proc wrap(pyobject arg) { wrapping res; res.p = arg; return (res); }
system("install", "pyobject", "wrapping", wrap, 1);

pyobject nineteen = 19;
wrapped = nineteen;
wrapped;


// interaction with built-ins
proc to_poly(pyobject arg) { return (poly(23)); }
system("install", "pyobject", "poly", to_poly, 1);

pyobject for_poly="4poly";
poly(for_poly);

proc from_poly(poly arg) { pyobject res = string(arg); return (res); }
system("install", "pyobject", "=", from_poly, 1);

poly p = x+1;
for_poly = p;
for_poly;


// interaction with both built-ins and newstructs
newstruct("another","pyobject p");
another other;
other.p = seventeen;
other;

released = other;  // -> error
released;

proc from_types(def arg) {
  if (typeof(arg) == "poly") { return (from_poly(arg)); }
  if (typeof(arg) == "another") { return (arg.p); }
}
system("install", "pyobject", "=", from_types, 1);

released = other;
released;
released = p;
released;

proc from_ideal(ideal arg) { pyobject res = "(missing)"; return (res); }
system("install", "pyobject", "=", from_ideal, 1);
pyobject ported = ringlist(r);
ported;

tst_status(1);
$
