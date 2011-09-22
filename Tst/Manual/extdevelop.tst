LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring=0,(x,y),dp;
list Hne=hnexpansion(x14-3y2x11-y3x10-y2x9+3y4x8+y5x7+3y4x6+x5*(-y6+y5)
-3y6x3-y7x2+y8);
displayHNE(Hne);    // HNE of 1st,3rd branch is finite
print(extdevelop(Hne[1],5)[1]);
list ehne=extdevelop(Hne[2],5);
displayHNE(ehne);
param(Hne[2]);
param(ehne);
tst_status(1);$
