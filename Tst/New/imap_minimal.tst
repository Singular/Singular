LIB "tst.lib";
tst_init(); option(Imap);

ring s0 = 32003, (a,b,x,y), lp;
poly p = 1 + a + a2 + b + ab + ax + ay + bx + x + y + xy; p;

ring r1 = (0, a), (b, x, y), lp;
poly p = 1 + a + a2 + b + ab + ax + ay + bx + x + y + xy; p;

ring r2 = (0, a, b), (x, y), lp;
poly p = 1 + a + a2 + b + ab + ax + ay + bx + x + y + xy; p;

setring s0;
imap (r1,p);
imap (r2,p);

tst_status(1); $
