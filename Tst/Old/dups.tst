int i = 9;
ring r;
diff(r,x);
poly i = x;
poly r = x;
diff(i,x);
diff(i,a);
diff(i+x,x);
poly p = xy;
ring rr;
int p = 9;
listvar(all);
diff(p,x);
listvar(all);
setring r;
"int p is shadowed by poly p!!";
diff(p,x);
LIB "tst.lib";tst_status(1);$
