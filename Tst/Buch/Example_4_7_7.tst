LIB "tst.lib";
tst_init();

//================================= example 4.7.7 ============================

LIB"primdec.lib";
ring R=0,(x,y,z),lp;
ideal I=z2-2, y2+2y-1, (y+z+1)*x+yz+z+2, x2+x+y-1;
minAssGTZ(I);

option(redSB);        //a reduced lex Groebner basis is needed
I=std(I);             //as input for triangMH (algorithm
triangMH(I,2);        //of Moeller, Hillebrand)

std(quotient(I,y+z+1));  //the second triangular set

std(I,y+z+1);            //the first triangular set
                         //(recall the meaning of std(I, f))


tst_status(1);$

