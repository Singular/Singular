LIB "tst.lib";
tst_init();

// ordering rp, attribute and monomial order

ring r0=0,x,rp;
1+x;
ring r1=0,(x,y),rp;
1+x+y;
attrib(r0,"global");
attrib(r1,"global");

tst_status(1);$
