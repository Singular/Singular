LIB "tst.lib";
tst_init();

LIB "primdecint.lib";

proc sortPDZ(list l)
{
   int i,j,notReady;
   list K;
   intvec save_opt=option(get);
   option(redSB,redTail);
   for(i=1;i<=size(l);i++){l[i][1]=std(l[i][1]);l[i][2]=std(l[i][2]);}
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


ring R1 = integer,(a,b,c,d,e,f,g),dp;
ideal I = 2*3,a2+2de+2cf+2bg+a, 2ab+e2+2df+2cg+b,
          b2+2ac+2ef+2dg+c, 2bc+2ad+f2+2eg+d,
          c2+2bd+2ae+2fg+e, 2cd+2be+2af+g2+f, d2+2ce+2bf+2ag+g;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));


ring R3 = integer,(w,z,y,x),dp;
ideal I = 2*3*181*32003, xzw+(-y^2+y)*z^2, (-x^2+x)*w^2+yzw,
          ((y^4-2*y^3+y^2)*x-y^4+y^3)*z^3,y2z2w+(-y*4+2*y^3-y^2)*z3;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R5 = integer,(x,y,z),dp;
ideal I = 2*13*181*32003, x2-y2-z2, xy-z2, y3+xz2-yz2+2z3+xy-z2,
          -y2z2+2z4+x2-y2+z2, y3z9+3y2z10+3yz11+z12-y2z2+2z4;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R7 = integer,(x,y,z),dp;
ideal I = 2^4*3*5*7*11*13*17, x2-y2-(z+2)^2, xy-(z+2)^2,
          y3+x*(z+2)^2-y*(z+2)^2+2*(z+2)^3+xy-(z+2)^2,
          -y^2*(z+2)^2+2*(z+2)^4+x2-y2+(z+2)^2,
          y3z9+3y2z10+3yz11+z12-y2z2+2z4;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,2));
sortPDZ(primdecZ(I,3));
sortPDZ(primdecZ(I,4));

ring R8 = integer,(x,y,z),dp;
ideal I = x2-y2-(z+2)^2, xy-(z+2)^2,
          y3+x*(z+2)^2-y*(z+2)^2+2*(z+2)^3+xy-(z+2)^2,
          -y^2*(z+2)^2+2*(z+2)^4+x2-y2+(z+2)^2,
          y3z9+3y2z10+3yz11+z12-y2z2+2z4;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R9 = integer,(w,z,y,x),dp;
ideal I = 2*3*5*630, ((y^2-y)*x-y^3+y^2)*z^2, (x-y)*zw,
          (x-y^2)*zw+(-y^2+y)*z^2, (-x^2+x)*w^2+(-yx+y)*zw;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R10 = integer,(w,z,y,x),dp;
ideal I = 2*3*5*1260, -yxzw+(-y^2+y)*z^2, (-x^2+x)*w^2-yxzw,
          ((-y^2+y)*x-y^3+2*y^2-y)*z^3,
          (y^2-y)*z^2*w+(-y^2+y)*z^2*w+(-y^2+y)*z^3;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R11 = integer,(w,z,y,x),dp;
ideal I = (4*y^2*x^2+(4*y^3+4*y^2-y)*x-y^2-y)*z^2,
          (x+y+1)*zw+(-4*y^2*x-4*y^3-4*y^2)*z^2,
          (-x-2*y^2 - 2*y - 1)*zw + (8*y^3*x + 8*y^4 + 8*y^3 + 2*y^2+y)*z^2,
          ((y^3 + y^2)*x - y^2 - y)*z^2,
          (y +1)*zw + (-y^3 -y^2)*z^2,
          (x + 1)*zw +(- y^2 -y)*z^2,
          (x^2 +x)*w^2 + (-yx - y)*zw;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,2));
sortPDZ(primdecZ(I,3));
sortPDZ(primdecZ(I,4));

ring R12 = integer,(w,z,y,x),dp;
ideal I = 2*3*181*72,
          ((y^3 + y^2)*x - y^2 - y)*z^2,
          (y + 1)*zw + (-y^3 -y^2)*z^2,
          (x + 1)*zw + (-y^2 -y)*z^2, (x^2 + x)*w^2 + (-yx - y)*zw;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R14 = integer,(x(1),x(2),x(3),x(4)),dp;
ideal I = 181*49^2, x(4)^4, x(1)*x(4)^3, x(1)*x(2)*x(4)^2,
          x(2)^2*x(4)^2, x(2)^2*x(3)*x(4), x(1)*x(2)*x(3)*x(4),
          x(1)*x(3)^2*x(4), x(3)^3*x(4);
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R15 = integer,(x,y,z),dp;
ideal I = 32003*181*64,
          ((z^2-z)*y^2 + (z^2 -z)*y)*x,
          (z*y^3 + z*y^2)*x,
          (y^4 - y^2)*x,
          (z^2 - z)*y*x^2,
          (y^3 - y^2)*x^2,
          (z^3 - z^2)*x^4 + (2*z^3 -2*z^2)*x^3 + (z^3 -z^2)*x^2,
          z*y^2*x^2;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R16 = integer,(x(1),x(2),x(3),x(4),x(5)),dp;
ideal I = x(5)^5, x(1)*x(5)^4, x(1)*x(2)*x(5)^3, x(2)^2*x(5)^3,
          x(2)^2*x(3)*x(5)^2, x(1)*x(2)*x(3)*x(5)^2, x(1)*x(3)^2*x(5)^2,
          x(3)^3*x(5)^2, x(3)^3*x(4)*x(5), x(1)*x(3)^2*x(4)*x(5),
          x(1)*x(2)*x(3)*x(4)*x(5), x(2)^2*x(3)*x(4)*x(5),
          x(2)^2*x(4)^2*x(5), x(1)*x(2)*x(4)^2*x(5), x(1)*x(4)^3*x(5),
          x(4)^4*x(5);
I = intersectZ(I,ideal(64*181,x(1)^2));
I;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,2));
sortPDZ(primdecZ(I,3));
sortPDZ(primdecZ(I,4));

ring R17 = integer,(x,y,z),dp;
ideal I = 374, (z+2)^8-140z6+2622*(z+2)^4-1820*(z+2)^2+169,
          17y*(z+2)^4-374*y*(z+2)^2+221y+2z7-281z5+5240z3-3081z,
          204y2+136yz3-3128yz+z6-149z4+2739z2+117,
          17xz4-374xz2+221x+2z7-281z5+5240z3-3081z,
          136xy-136xz-136yz+2z6-281z4+5376z2-3081,
          204x2+136xz3-3128xz+z6-149z4+2739z2+117;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R24 = integer,(a,b,c,d,e,f,g,h,j),dp;
ideal I = 2*3*5*7*11*13*17, a+2b+c-d+g, f2gh-a, efg-c, fg2j-b,
          a+b+c+f+g-1, 3ad+3bd+2cd+df+dg-a-2b-c-g;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

ring R25 = integer,(s,r,b,u,v,w,x,y,z),dp;
ideal I = 2*3*5*7*11*13*17*19*23, su+bv, ru+bw, rv+sw, sx+by, rx+bz,
          ry+sz, vx+uy, wx+uz, wy+vz;
sortPDZ(primdecZ(I,1));
sortPDZ(primdecZ(I,3));

tst_status(1);$
