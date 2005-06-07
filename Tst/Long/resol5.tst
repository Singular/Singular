LIB "tst.lib";
tst_init();

LIB"reszeta.lib";

"============== U16";
ring R=0,(x,y,z),dp;
ideal I=y5+x2y2+x3-xz2;  //U16 
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== J3,0";
ring R=0,(x,y,z),dp;
ideal I=x3+x2y3+y9+z2;;  //J3,0
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;


"============== J3,1";
//ring R=0,(x,y,z),dp;
//ideal I=x3+x2y3+y10+z2;   //J3,1  long
//list L=resolve(I);
//zetaDL(L,1);
//kill L;kill R;


"============== Z1,0";
ring R=0,(x,y,z),dp;
ideal I=x3y+x2y3+y7+z2;   //Z1,0
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;


"============== S1,1";
ring R=0,(x,y,z),dp;
ideal I=x2z+yz2+x2y2+y6;  //S1,1
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;


"============== E18";
//ring R=0,(x,y,z),dp;
//ideal I=x3+y10+xy7+z2;  //E18  ERROR in RADICAL2
//list L=resolve(I);
//zetaDL(L,1);
//kill L;kill R;


"============== W17";
ring R=0,(x,y,z),dp;
ideal I=x4+xy5+y7+z2;  //W17   
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;


"============== D10";
//ring R=0,(x,y,z),dp;
//ideal I=x2y+y9+z2;  //D10  long
//list L=resolve(I);
//zetaDL(L,1);
//kill L;kill R;


"============== A10";
ring R=0,(x,y,z),dp;
ideal I=x11+y2+z2;  //A10
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;


"============== A15";
ring R=0,(x,y,z),dp;
ideal I=x16+y2+z2;  //A15
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;


"============== (y2-x3)^2-x7+z2";
ring R=0,(x,y,z),dp;
ideal I=(y2-x3)^2-x7+z2;  
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x4+y4+z4+xyz2+x2yz2";
ring R=0,(x,y,z),dp;
ideal I=x4+y4+z4+xyz2+x2yz2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x4+y5+z6+xyz2+x2yz2";
ring R=0,(x,y,z),dp;
ideal I=x4+y5+z6+xyz2+x2yz2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x4+y4+z6+xyz2+x2yz2+x2y2z";
ring R=0,(x,y,z),dp;
ideal I=x4+y4+z6+xyz2+x2yz2+x2y2z;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x4+y4+z6+y2z2+xyz";
ring R=0,(x,y,z),dp;
ideal I=x4+y4+z6+y2z2+xyz;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x4+y4+z6+x2y2";
ring R=0,(x,y,z),dp;
ideal I=x4+y4+z6+x2y2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x4+y4+z6+x4y+xy4+xyz";
ring R=0,(x,y,z),dp;
ideal I=x4+y4+z6+x4y+xy4+xyz;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x6+y6+z4+x4y+x2y2";
ring R=0,(x,y,z),dp;
ideal I=x6+y6+z4+x4y+x2y2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x6+y6+z4+x2y2";
ring R=0,(x,y,z),dp;
ideal I=x6+y6+z4+x2y2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x6+y6+z4+xy4+x4y+xyz";
ring R=0,(x,y,z),dp;
ideal I=x6+y6+z4+xy4+x4y+xyz;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x6+y6+z4+xy4+x4y+xyz+x2y2";
ring R=0,(x,y,z),dp;
ideal I=x6+y6+z4+xy4+x4y+xyz+x2y2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x6+y6+z2+x2y2";
ring R=0,(x,y,z),dp;
ideal I=x6+y6+z2+x2y2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x6+y6+z2+xy4+x4y+xyz";
ring R=0,(x,y,z),dp;
ideal I=x6+y6+z2+xy4+x4y+xyz;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

"============== x6+y6+z4+xy4+x4y+xyz2";
//ring R=0,(x,y,z),dp;
//ideal I=x6+y6+z4+xy4+x4y+xyz2;
//list L=resolve(I);
//zetaDL(L,1);

"============== x7+y8+z8+x2y2z2";
ring R=0,(x,y,z),dp;
ideal I=x7+y8+z8+x2y2z2;
list L=resolve(I);
zetaDL(L,1);
kill L;kill R;

tst_status(1);$
