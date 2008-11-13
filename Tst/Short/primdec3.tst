LIB "tst.lib";
tst_init();

LIB"primdec.lib";
proc sortMinAss(list l)
{
   int i,j,notReady;
   ideal K;
   intvec save=option(get);option(redSB);
   for(i=1;i<=size(l);i++){"start interrred";l[i]=interred(l[i]);"end interred";}
 
   
   notReady=1;
   while(notReady)
   {
      notReady=0;
      i=0;
      while(i<size(l)-1)
      {
         i++;
         j=i;
         while(j<size(l))
         {
            j++;
            if(compareI(l[i],l[j]))
            {
               notReady=1;
               K=l[i];
               l[i]=l[j];
               l[j]=K;
               i--;
               break;
            }
         }
      }
   }
   option(set,save);
   return(l);
} 
proc sortGTZ(list l)
{
   int i,j,notReady;
   list K;
   intvec save_opt=option(get);
   option(redSB,redTail);
   for(i=1;i<=size(l);i++){"start";l[i][1]=std(l[i][1]);l[i][2]=std(l[i][2]);"end";}
   option(set,save_opt); 
   notReady=1;
   while(notReady)
   {
      notReady=0;
      i=0;
      while(i<size(l)-1)
      {
         i++;
         j=i;
         while(j<size(l))
         {
            j++;
            if(compareI(l[i][2],l[j][2]))
            {
               notReady=1;
               K=l[i];
               l[i]=l[j];
               l[j]=K;
               i--;
               break;
            }
         }
      }
   }
   return(l);  
}
proc compareI(ideal I, ideal J)
{
   return(string(I)>string(J));
}


printlevel =0;

// 1. Beispiel
example zerodec;

//2. Beispiel  (2sec)
ring r1= 0,(a,b,c),dp;
poly f1= a^2*b*c + a*b^2*c + a*b*c^2 + a*b*c + a*b + a*c + b*c;
poly f2= a^2*b^2*c + a*b^2*c^2 + a^2*b*c + a*b*c + b*c + a + c;
poly f3= a^2*b^2*c^2 + a^2*b^2*c + a*b^2*c + a*b*c + a*c + c + 1;
ideal gls=f1,f2,f3;
sortMinAss(zerodec(gls));
sortGTZ(primdecGTZ(gls));

//3. Beispiel  (5sec)
ring rs=0,(x1,x2,x3,x4),dp;
poly f1=16*x1^2 + 3*x2^2 + 5*x3^4 - 1 - 4*x4 + x4^3;
poly f2=5*x1^3 + 3*x2^2 + 4*x3^2*x4 + 2*x1*x4 - 1 + x4 + 4*x1 + x2 + x3 + x4;
poly f3=-4*x1^2 + x2^2 + x3^2 - 3 + x4^2 + 4*x1 + x2 + x3 + x4;
poly f4=-4*x1 + x2 + x3 + x4;
ideal gls=f1,f2,f3,f4;
sortMinAss(zerodec(gls));
sortGTZ(primdecGTZ(gls));

//4. Beispiel  (23sec)
ring r2= 0,(y,z,x,t),dp;
ideal gls=y^2*z+2*y*x*t-z-2*x,
4*y^2*z*x-z*x^3+2*y^3*t+4*y*x^2*t-10*y^2+4*z*x+4*x^2-10*y*t+2,
2*y*z*t+x*t^2-2*z-x,
-z^3*x+4*y*z^2*t+4*z*x*t^2+2*y*t^3+4*z^2+4*z*x-10*y*t-10*t^2+2;
sortMinAss(zerodec(gls));
sortGTZ(primdecGTZ(gls));

//5. cyclic5  vdim=70, 
//zerodec-time:49  (matrix:0 charpoly:25 factor:23)
//primdecGTZ-time: 28
ring rs= 0,(a,b,c,d,e),dp;
poly f0= a + b + c + d + e + 1;
poly f1= a + b + c + d + e;
poly f2= a*b + b*c + c*d + a*e + d*e;
poly f3= a*b*c + b*c*d + a*b*e + a*d*e + c*d*e;
poly f4= a*b*c*d + a*b*c*e + a*b*d*e + a*c*d*e + b*c*d*e;
poly f5= a*b*c*d*e - 1;
ideal gls= f1,f2,f3,f4,f5;
sortMinAss(zerodec(gls));
sortGTZ(primdecGTZ(gls));

tst_status(1);$
