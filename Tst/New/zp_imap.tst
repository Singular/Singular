LIB "tst.lib"; tst_init();

ring R = (0),(x,y,z),(ds(3),C);
poly p = -7/5;

ring @R = (1000000007),(x,y,z),(ds(3),C);

(-7/5) == 200000000;

number(400000003) * number(1000000000) == number(200000000);

poly p = imap(R, p);

p; // WRONG under 32 bit :(
p == 200000000;
p*5 + 7; // must be 0!



tst_status(1);$
