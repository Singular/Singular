LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x(1),y(1),x(2),y(2),x(3),y(3),x(4),y(4)),dp;
def R = setLetterplaceAttributes(r,4,2); // supply R with letterplace structure
setring R;
poly a = x(1)*y(2); poly b = y(1);
lieBracket(a,b);
lieBracket(x(1),y(1),2);
tst_status(1);$
