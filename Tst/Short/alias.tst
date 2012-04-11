LIB "tst.lib";
tst_init();

proc probe(alias list args)
{
args;
typeof(args);
int i = size(args[1]);
}

ring r = 0, x, dp;
ideal i = x;
list l = list(i, 2);

probe(l);

proc probe2(alias list i) { i; };

list n = 1;
probe2(n);

tst_status(1);$

