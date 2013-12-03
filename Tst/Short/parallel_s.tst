LIB "tst.lib";
tst_init();

LIB "parallel.lib";

ring R1 = 0, (x,y,z), lp;
ideal I = 3x3y+x3+xy3+y2z2, 2x3z-xy-xz3-y4-z2, 2x2yz-2xy2+xz2-y4;
ideal J = x10+x9y2, x2y7-y8;
list commands = list("std", "std");
list arguments = list(list(I), list(J));
parallelWaitN(commands, arguments, 1);
parallelWaitFirst(commands, arguments);

ring R2 = 0, (x,y,z), dp;
ideal I1 = z8+z6+4z5+4z3+4z2+4, -z2+y;
ideal I2 = x9y2+x10, x2y7-y8;
ideal I3 = x3-2xy, x2y-2y2+x;
string command = "std";
list arguments = list(list(I1), list(I2), list(I3));
parallelWaitAll(command, arguments);

ring R3 = 0, (x,y,z), dp;
ideal I = x, y, z;
intvec v = 0:3;
list l = list(I, v);
module m1 = x*gen(1);
module m2;
command = "size";
list arguments1 = list(list(I), list(v), list(l), list(m1));
list arguments2 = list(list(I), list(v), list(l), list(m2));
parallelTestAND(command, arguments1);
parallelTestAND(command, arguments2);

ring R4 = 0, (x,y,z), dp;
ideal I;
string s;
list l;
module m1 = x*gen(1);
module m2;
command = "size";
list arguments1 = list(list(I), list(s), list(l), list(m1));
list arguments2 = list(list(I), list(s), list(l), list(m2));
parallelTestOR(command, arguments1);
parallelTestOR(command, arguments2);

tst_status(1);$
