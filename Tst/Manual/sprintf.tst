LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
module m=[1,y],[0,x+z];
intmat M=betti(mres(m,0));
list l = r, m, M;
string s = sprintf("s:%s,%n l:%l", 1, 2); s;
s = sprintf("s:%n%s", l); s;
s = sprintf("s:%2%s", list(l)); s;
s = sprintf("2l:%n%2l", list(l)); s;
s = sprintf("%p", list(l)); s;
s = sprintf("%;", list(l)); s;
s = sprintf("%b", M); s;
tst_status(1);$
