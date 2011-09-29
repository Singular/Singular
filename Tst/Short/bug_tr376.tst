LIB "tst.lib";
tst_init();

 // overflow while reading a monomial

 ring r=0,(x,y,t),dp;
 poly p1 = 3842432640y5t3;
 ring s=536870909,(x,y,t),dp;
 poly p2 = 3842432640y5t3;
 poly p1 = fetch(r, p1);
 p1-p2;

 ring rr=0,(x,y,t),dp;
 poly p1 = 3842432640*y5t3;
 ring ss=536870909,(x,y,t),dp;
 poly p2 = 3842432640*y5t3;
 poly p1 = fetch(rr, p1);
 p1-p2;


tst_status(1);$
