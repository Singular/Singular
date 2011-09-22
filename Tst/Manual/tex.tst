LIB "tst.lib"; tst_init();
LIB "latex.lib";
ring r;
ideal I = maxideal(7);
opentex("exp001");              // open latex2e document
texobj("exp001","An ideal ",I);
closetex("exp001");
tex("exp001");
system("sh","rm exp001.*");
tst_status(1);$
