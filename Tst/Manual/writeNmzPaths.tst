LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
setNmzExecPath("../Normaliz/");
writeNmzPaths();
int dummy=system("sh","cat nmz_sing_exec.path");
dummy=system("sh","cat nmz_sing_data.path");
tst_status(1);$
