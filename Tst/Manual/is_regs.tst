LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;
ring r1    = 32003,(x,y,z),ds;
ideal i    = x8,y8,(x+y)^4;
is_regs(i);
module m   = [x,0,y];
i          = x8,(x+z)^4;;
is_regs(i,m);
printlevel = p;
tst_status(1);$
