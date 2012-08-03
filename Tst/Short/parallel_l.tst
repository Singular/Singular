LIB "tst.lib";
tst_init();

LIB "parallel.lib";
LIB "primdec.lib";

ring r = 0, (x,y,z), lp;
ideal i = z8+z6+4z5+4z3+4z2+4, y-z2;
ideal j = 3x3y+x3+xy3+y2z2, 2x3z-xy-xz3-y4-z2, 2x2yz-2xy2+xz2-y4;
list commands = list("std", "primdecGTZ", "primdecSY",
                     "std", "primdecGTZ", "primdecSY");
list args = list(list(i), list(i), list(i),
                 list(j), list(j), list(j));
parallelWaitN(commands, args, 3);

commands = list("primdecGTZ", "primdecSY");
args = list(list(i), list(i));
parallelWaitFirst(commands, args);

ring s = 0, (x,y,z), dp;
ideal i1 = z8+z6+4z5+4z3+4z2+4, y-z2;
ideal i2 = x10+x9y2, y8-x2y7;
ideal i3 = x3-2xy, x2y-2y2+x;
string command = "std";
list args = list(list(i1), list(i2), list(i3));
parallelWaitAll(command, args);

tst_status(1);$
