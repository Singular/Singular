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

tst_status(1);
$
