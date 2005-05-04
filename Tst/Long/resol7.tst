LIB "tst.lib";
tst_init();

LIB"zeta.lib";
"============== A1";
ring R=0,(x,y,z),dp;
ideal I=x2+yz;   // A_1
list L=resolve(I,"A");
zetaDL(L,1,"local");
zetaDL(L,2);
kill L;kill R;

"============== A2";
ring R=0,(x,y,z),dp;
ideal I=x2-y2+z3; // A_2
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== A3";
ring R=0,(x,y,z),dp;
ideal I=x2-y2+z4; // A_3
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== E6";
ring R=0,(x,y,z),dp;
ideal I=x3+y4-z2; // E_6
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== E7";
//ring R=0,(x,y,z),dp;
//ideal I=x3+xy3-z2; // E_7  long
//list L=resolve(I,"A");
//zetaDL(L,1,"local");
//kill L;kill R;

"============== E8";
//ring R=0,(x,y,z),dp;
//ideal I=x3+y5-z2; // E_8 long
//list L=resolve(I,"A");
//zetaDL(L,1,"local");
//kill L;kill R;

"============== P8";
ring R=0,(x,y,z),dp;
ideal I=x3+y3-z3; // P_8
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== X9";
ring R=0,(x,y,z),dp;
ideal I=x4-y4+z2; // X_9
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== J10";
ring R=0,(x,y,z),dp;
ideal I=x3+y6-z2; // J_10
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== D4";
ring R=0,(x,y,z),dp;
ideal I=x2y-y3+z2; // D_4
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== D5";
ring R=0,(x,y,z),dp;
ideal I=x2y-y4+z2; // D_5
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== D6";
ring R=0,(x,y,z),dp;
ideal I=x2y-y5+z2; // D_6        
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== D7";
ring R=0,(x,y,z),dp;
ideal I=x2y-y6+z2; // D_7
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== T444";
ring R=0,(x,y,z),dp;
ideal I=x4-y4+z4+xyz; // T_4,4,4
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== T343";
ring R=0,(x,y,z),dp;
ideal I=x2y+y4+z3;
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== T443";
//ring R=0,(x,y,z),dp;  //hier gab es Probleme mit gcd
//ideal I=x4-y4+z3;
//list L=resolve(I,"A");
//zetaDL(L,1,"local");
//kill L;kill R;

"============== W12";
ring R=0,(x,y,z),dp;
ideal I=x2y3+y5+x4+z2; //W12
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== T345";
ring R=0,(x,y,z),dp;
ideal I=x3+y4+z5+xyz; // T3,4,5
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== U12";
ring R=0,(x,y,z),dp;
ideal I=xyz2+z4+x3+y3; // U12
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== U1,0";
ring R=0,(x,y,z),dp;
ideal I=xy3+y3z+x3+xz2; // U1,0
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== W13";
ring R=0,(x,y,z),dp;
ideal I=y6+xy4+x4+z2; // W13
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== Z12";
ring R=0,(x,y,z),dp;
ideal I=x2y3+xy4+x3y+z2; // Z12
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== Z13";
ring R=0,(x,y,z),dp;
ideal I=xy5+y6+x3y+z2; // Z13
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== Z11";
ring R=0,(x,y,z),dp;
ideal I=xy4+y5+x3y+z2; // Z11   
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== xyz";
ring R=0,(x,y,z),dp;
ideal I=xyz;
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== xy";
ring R=0,(x,y,z),dp;
ideal I=xy;
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== xz(xy+z)";
ring R=0,(x,y,z),dp;
ideal I=xz*(xy+z);
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== x2y2z+xyz2";
ring R=0,(x,y,z),dp;
ideal I=x2y2z+xyz2;
list L=resolve(I,"A");
zetaDL(L,1,"local");
zetaDL(L,2);
kill L;kill R;

"============== Cayley cubic";
ring R=0,(x,y,z),dp;
ideal I=xy+xz+yz+xyz;  //Cayley cubic 
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== E12";
ring R=0,(x,y,z),dp;
ideal I=y7+xy5+x3+z2;  //E12    
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== E13";
ring R=0,(x,y,z),dp;
ideal I=y8+xy5+x3+z2;  //E13  
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== E14";
ring R=0,(x,y,z),dp;
ideal I=y8+xy6+x3+z2;  //E14 
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== Q10";
ring R=0,(x,y,z),dp;
ideal I=xy3+y4+x3+yz2;  //Q10
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== Q11";
ring R=0,(x,y,z),dp;
ideal I=z5+xz3+x3+y2z;  //Q11
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== Q12";
ring R=0,(x,y,z),dp;
ideal I=xy4+y5+x3+yz2;  //Q12 
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== S11";
ring R=0,(x,y,z),dp;
ideal I=x4+x3z+y2z+xz2;  //S11
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;

"============== S12";
ring R=0,(x,y,z),dp;
ideal I=z5+xz3+x2y+y2z;  //S12  
list L=resolve(I,"A");
zetaDL(L,1,"local");
kill L;kill R;


tst_status(1);$
