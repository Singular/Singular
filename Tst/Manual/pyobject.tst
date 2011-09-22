LIB "tst.lib"; tst_init();
pyobject pystr = "Hello";
pyobject pyint = 2;
string singstr = string(pystr + " World!");
singstr;
pystr + pyint;  // Error: not possible
pystr * pyint;  // But this is allowed,
pystr * 3;      // as well as this;

python_run("def newfunc(*args): return list(args)");  // syncs contexts!
newfunc(1, 2, 3);          // newfunc also knowd to SINGULAR

def pylst = python_eval("[3, 7, 1]");
proc(attrib(pylst, "sort"))(); // Access python member routines as attributes
pylst.sort();             // <- equivalent short-notation
pylst."sort"();           // <- alternative short-notation
pylst;

python_import("os");       // Gets stuff from python module 'os'
name;                      // The identifier of the operating system
tst_status(1);$
