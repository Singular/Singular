LIB "tst.lib";
tst_init();
// an error about side effects of WerrorS in libfac routines:

LIB "primdec.lib";
ring r = 0,(x,y,z),lp;
def p=z^2 + 1;
def q=z^3 + 2;
def sage20=p * q^2;
def sage21=y-z^2;
ideal i=sage20,sage21;
kill sage21;
kill sage20;
def sage23=primdecSY(i);
print(sage23);

ring r = 0,(x,y,z),lp;
def sage24=z^2 + 1;
ERROR("kill p;");
def sage25=z^3 + 2;
def sage26=2;
def sage27=sage25 ^ sage26;
def sage28=sage24 * sage27;
def sage29=y-z^2;
ideal sage30=sage28,sage29;
def sage31=primdecSY(sage30);
sage31;

tst_status(1);$
