LIB "tst.lib";
tst_init();

// ordering ip, attribute and monomial order

ring r0=0,x,ip;
1+x;
ring r1=0,(x,y),ip;
1+x+y;
attrib(r0,"global");
attrib(r1,"global");

tst_status(1);$
