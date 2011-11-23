LIB "tst.lib"; tst_init();

option(noloadLib);
ring quring = (0,D), (A,B,C), (C,lp);
LIB "primdec.lib";
ideal @j = C, DB+D2, DA;
ideal ser = DC, DA, DB+D18;
attrib(@j, "isSB", 1);
list uprimary = zero_decomp(@j, ser, 0);
//uprimary;   // if you uncomment this line, the error does not occur
ring gnir = 0, (A,B,C,D), (C,lp);
imap(quring, uprimary);

tst_status(1);$
