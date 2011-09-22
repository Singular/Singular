LIB "tst.lib"; tst_init();
LIB "inout.lib";
// Assume there exists a file 'Macid' with the following ideal in
// Macaulay format:"
// x[0]3-101/74x[0]2x[1]+7371x[0]x[1]2-13/83x[1]3-x[0]2x[2] \
//     -4/71x[0]x[1]x[2]
// Read this file into Singular and assign it to the string s1 by:
// string s1 = read("Macid");
// This is equivalent to";
string s1 =
"x[0]3-101/74x[0]2x[1]+7371x[0]x[1]2-13/83x[1]3-x[0]2x[2]-4/71x[0]x[1]x[2]";
rMacaulay(s1);
// You may wish to assign s1 to a Singular ideal id:
string sid = "ideal id =",rMacaulay(s1),";";
ring r = 0,x(0..3),dp;
execute(sid);
id; "";
// Now treat a matrix in Macaulay format. Using the execute
// command, this could be assinged to a Singular matrix as above.
string s2 = "
0  0  0  0  0
a3 0  0  0  0
0  b3 0  0  0
0  0  c3 0  0
0  0  0  d3 0
0  0  0  0  e3 ";
rMacaulay(s2);
tst_status(1);$
