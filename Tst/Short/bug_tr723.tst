LIB "tst.lib";
tst_init();

 ring r = (0,a), (x,y,z), dp;
 module m = (a)*x*gen(1)+536870909*y*gen(3);
 m;
 ring s = (536870909,a), (x,y,z), dp;
 module m = fetch(r, m);
 m;
 m;

 setring r; m;
 setring s; m;
 kill r;
 kill s;
 //-------------------------------------------
 ring r1 = (0,a), (x,y,z), dp;
 module m = (a)*x*gen(1)+536870909*y*gen(3);
 m;
 module m2=m;
 m;
 m2;
 kill r1;
 //-------------------------------------------

 ring r2 = (536870909,a), (x,y,z), dp;
 module m = (a)*x*gen(1)+536870909*y*gen(3);
 m;
 module m2=m;
 m;
 m2;
 kill r2;
 //-------------------------------------------
 ring S = (integer, 103),x,dp;

 ideal I = x;
 ring R = integer,y,dp;
 fetch (S,I);
 kill R; kill S;
 //-------------------------------------------

ring r = (0,a), (x,y,z), dp;
module m1 = (a)*x*gen(1)+536870909*y*gen(3), y*gen(2), z*gen(3);
module m2 = x*gen(1)+gen(1), x*gen(1)+gen(1), x*gen(1)+gen(1);
ring rp = (536870909, a), (x,y,z), dp;
module m1 = fetch(r, m1);
module m2 = fetch(r, m2);
m1,
m2;
tst_status(1);$
