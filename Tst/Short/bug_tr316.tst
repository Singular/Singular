LIB "tst.lib";
tst_init();

// output should be suitable for input
ring Rc =complex,x,dp;
poly f = 0.0123*x2+0.03;
f;
execute("poly g="+string(f)+";");
f==g;

ring R = real,x,dp;
poly f = 0.0123*x2+0.03;
f;
execute("poly g="+string(f)+";");
f==g;

ring R10 =(real,20,10),x,dp;
poly f = 0.0123*x2+0.03;
f;
execute("poly g="+string(f)+";");
f==g;

tst_status(1);$

