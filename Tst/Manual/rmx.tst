LIB "tst.lib"; tst_init(); /* tst_ignore("This is pdfTeX"); */ /* tst_ignore("Babel"); */
LIB "latex.lib";
ring r;
poly f = x+y+z;
opentex("exp001");              // defaulted latex2e document
texobj("exp001","A polynom",f);
closetex("exp001");
tex("exp001");
rmx("exp001");   // removes aux and log file of exp001
system("sh","rm exp001.*");
tst_status(1);$
