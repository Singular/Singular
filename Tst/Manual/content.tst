LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r=0,(x,y,z),(c,lp);
content(3x2+18xy-27xyz);
vector v=[3x2+18xy-27xyz,15x2+12y4,3];
content(v);
tst_status(1);$
