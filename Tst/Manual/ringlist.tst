LIB "tst.lib"; tst_init();
 ring r = 0,(x(1..3)),dp;
 list l = ringlist(r);
 l;
 //  Now change l and create a new ring, by
 //- changing the base field to the function field with parameter a,
 //- introducing one extra variable y,
 //- defining the block ordering (dp(2),wp(3,4)).
 //- define the minpoly after creating the function field
 l[1]=list(0,list("a"),list(list("lp",1)),ideal(0));
 l[2][size(l[2])+1]="y";
 l[3][3]=l[3][2]; // save the module ordering
 l[3][1]=list("dp",intvec(1,1));
 l[3][2]=list("wp",intvec(3,4));
 def ra = ring(l);     //creates the newring
 ra; setring ra;
 list lra = ringlist(ra);
 lra[1][4]=ideal(a2+1);
 def Ra = ring(lra);
 setring Ra; Ra;
tst_status(1);$
