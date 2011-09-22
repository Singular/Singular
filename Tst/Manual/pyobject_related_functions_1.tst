LIB "tst.lib"; tst_init();
python_run("def new_pyobj(): pass");
attrib(new_pyobj, "new_attr", "something");
attrib(new_pyobj, "new_attr");
attrib(new_pyobj);

killattrib(new_pyobj, "new_attr");
attrib(new_pyobj);
tst_status(1);$
