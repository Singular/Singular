LIB "tst.lib"; tst_init();
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
pystr[1];

python_run("def newfunc(*args): return list(args)");
newfunc();
newfunc(two, three);

newfunc."__class__";
newfunc::"__class__";
newfunc.func_name;
newfunc::func_name;

tst_status(1);$
