LIB "tst.lib";
tst_init();
// setting OrdSgn for matrix ordering with 0 in first row:
intmat m[3][3] = 3, 0, 1, 0, 1, 0, 0, 0, 1;
ring R = 0, (x,y,z), M(m);

attrib(R,"global");

tst_status(1);$
