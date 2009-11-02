LIB "tst.lib";
LIB "graphics.lib";

tst_init(); 
tst_ignore("CVS ID $Id$");
 
 // test staircase

  ring r0 = 0,(x,y),ls;
  ideal I = -1x2y6-1x4y2, 7x6y5+1/2x7y4+6x4y6;
  staircase("",std(I)); 
  ring r1 = 0,(x,y),dp;
  ideal I = fetch(r0,I);
  staircase("",std(I));
  ring r2 = 0,(x,y),wp(2,3);
  ideal I = fetch(r0,I);
  staircase("",std(I));

 // test math.init
  mathinit();
 
 // test mplot
   ring rr0 = 0,x1,dp; 
   ideal I = x1^3 + x1, x1^2;
   ideal J = x1^2,-x1+x1^3; "";
   mplot("",J);
   mplot("",I,"-2,2",J);

   ring rr1 = 0,x,dp;  
   ideal I(1) = 2x2-1/2x3 +1, x-1;
   ideal I(2) = x3-x ,x; 
   mplot("",I(1),I(2),"-2,4");
   I(1) = x3,-1/10x3+x2,x2;
   I(2) = x2,-x2+2,-2x2-x+1; 
   mplot("",I(1),I(2));
   
   ring rr = 0,(x,y),ds; 
   ideal I(1) = y,-x2;
   ideal I(2) = x2,-y2 +4; 
   ideal I(3) = x4+2x2y2 + y4, x2-y2;  
   poly f = (x-y)*(x2+y);
   mplot("",f); 
   ideal J = jacob(f);
   J; 
   mplot("",J[1],J[2]);  
   mplot("",J[1],J[2],"-10,10","-10,10");  
   mplot("",I(1),I(2),"-2.5,2.5"); 
   mplot("",I(3),"-2,2");
   ideal J(1) = 3xy4 + 2xy2, x5y3 + x + y6,10x2;
   ideal J(2) = xy,y,x2; "";
   mplot("",J(1),"-2,1","1,2"); 
   mplot("",J(2),"-1.5,1.5","-2,2");
 $

