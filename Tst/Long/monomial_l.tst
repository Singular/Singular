LIB "tst.lib";
tst_init();

LIB "monomialideal.lib";

// EX. 1:
ring r4=0,x(1..4),dp;
ideal i=x(1)^3*x(2)*x(3),x(1)*x(2)*x(3)*x(4),x(2)^2*x(3)^2*x(4)^2,x(2)^2*x(4)^4,x(3)^3*x(4);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);
list id=irreddecMon(i);
list id1=irreddecMon(i,"vas");

list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

list id5=irreddecMon(i,"mil");

list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX. 2:
ring r5=0,(x,y,z,t,w),dp;
ideal I=y2t,x2y-x2z+yt2,x2y2,xyztw,x3z2,y5+xz3w-x2zw2,x7-yt2w4;
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

//  list id1=irreddecMon(i,"vas");
list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

//  list id5=irreddecMon(i,"mil");
list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id2);size(id3);size(id4);size(id6);size(id8);

areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 3:
ring r6=0,x(0..5),dp;
ideal I=x(2)^2-x(4)*x(5),x(1)*x(2)-x(0)*x(5),x(0)*x(2)-x(1)*x(4),
          x(1)^2-x(3)*x(5),x(0)*x(1)-x(2)*x(3),x(0)^2-x(3)*x(4);
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

list id1=irreddecMon(i,"vas");

list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

list id5=irreddecMon(i,"mil");

list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 4
//
ring R4=0,(x,y,z,t),dp;
// Ex.2.5 in [Bermejo-Gimenez], Proc.Amer.Math.Soc. 128(5):
ideal I  = x17y14-y31, x20y13, x60-y36z24-x20z20t20;
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

list id1=irreddecMon(i,"vas");

list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

list id5=irreddecMon(i,"mil");

list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 5
// Ex.2.9 in [Bermejo-Gimenez], Proc.Amer.Math.Soc. 128(5):
int k=315;
I=x17y14-y31,x20y13,x60-y36z24-x20z20t20,y41*z^k-y40*z^(k+1);
i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

pd=primdecMon(i);

id=irreddecMon(i);

id1=irreddecMon(i,"vas");

id2=irreddecMon(i,"gz");

id3=irreddecMon(i,"ad");

id4=irreddecMon(i,"for");

id5=irreddecMon(i,"mil");

id6=irreddecMon(i,"lr");

id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 6
// Example in Rk.2.10 in [Bermejo-Gimenez], ProcAMS 128(5):
I=x2-3xy+5xt,xy-3y2+5yt,xz-3yz,2xt-yt,y2-yz-2yt;
i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

pd=primdecMon(i);

id=irreddecMon(i);

id1=irreddecMon(i,"vas");

id2=irreddecMon(i,"gz");

id3=irreddecMon(i,"ad");

id4=irreddecMon(i,"for");

id5=irreddecMon(i,"mil");

id6=irreddecMon(i,"lr");

id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 7
I=y4-t3z, x3t-y2z2, x3y2-t2z3, x6-tz5;
i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

pd=primdecMon(i);

id=irreddecMon(i);

id1=irreddecMon(i,"vas");

id2=irreddecMon(i,"gz");

id3=irreddecMon(i,"ad");

id4=irreddecMon(i,"for");

id5=irreddecMon(i,"mil");

id6=irreddecMon(i,"lr");

id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 8
ring R5=0,(x,y,z,t,w),dp;
ideal I = xy-zw,x3-yw2,x2z-y2w,y3-xz2,-y2z3+xw4+tw4+w5,-yz4+x2w3+xtw3+xw4,
           -z5+x2tw2+x2w3+yw4;
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

list id1=irreddecMon(i,"vas");

list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

list id5=irreddecMon(i,"mil");

list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 9
// Next example is the defining ideal of the 2nd. Veronesean of P3, a variety
// in P8 which is arithmetically Cohen-Macaulay:
ring r14=0,(a,b,c,d,x(0..9)),dp;
ideal i= x(0)-ab,x(1)-ac,x(2)-ad,x(3)-bc,x(4)-bd,x(5)-cd,
           x(6)-a2,x(7)-b2,x(8)-c2,x(9)-d2;
ideal ei=eliminate(i,abcd);
ring r10=0,x(0..9),dp;
ideal I=imap(r14,ei);
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

list id1=irreddecMon(i,"vas");

list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

list id5=irreddecMon(i,"mil");

list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 10:
ring r7=0,(x,y,z,t,u,a,b),dp;
ideal i=u-b40,t-a40,x-a23b17,y-a22b18+ab39,z-a25b15;
ideal ei=eliminate(i,ab); // It takes a few seconds to compute the ideal
ring rr5=0,(x,y,z,t,u),dp;
ideal I=imap(r7,ei);
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

list id1=irreddecMon(i,"vas");

list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

list id5=irreddecMon(i,"mil");

list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id1);areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 11:
ring r11=0,(x(0..8),s,t),dp;
ideal i=x(0)-st24,x(1)-s2t23,x(2)-s3t22,x(3)-s9t16,x(4)-s11t14,x(5)-s18t7,
          x(6)-s24t,x(7)-t25,x(8)-s25;
ideal ei=eliminate(i,st);
ring r9=0,x(0..8),dp;
ideal I=imap(r11,ei);
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

//  list id1=irreddecMon(i,"vas");
list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

//  list id5=irreddecMon(i,"mil");
list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id2);size(id3);size(id4);size(id6);size(id8);
// TARDA UN POCO PERO LO HACE
;

areEqual(id,id2);areEqual(id,id3);areEqual(id,id4);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 12:
ring r18=0,(s,t,u,x(0..14)),dp;
ideal i=x(0)-s3t6u6,x(1)-s5t5u5,x(2)-t9s6,x(3)-st4u10,x(4)-s2t12u,x(5)-t14u,
          x(6)-s8t7,x(7)-s4t6u5,x(8)-t13su,x(9)-s4t9u2,x(10)-stu13,x(11)-s5t7u3,
          x(12)-s15,x(13)-t15,x(14)-u15;
ideal ei=eliminate(i,stu);
ring r15=0,x(0..14),dp;
ideal I=imap(r18,ei);
ideal i=simplify(lead(std(I)),1);
size(i);

isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);  size(pd); // 2 SEGUNDOS CON "add", 4 componentes
;

list id=irreddecMon(i);  size(id); // 1 SEGUNDO CON "add", 180 componentes
;

list id1=irreddecMon(i,"sr");  // 103 SEC. CON "sr", coincide con la anterior
size(id1); areEqual(id,id1);

list id2=irreddecMon(i,"for");   // 19 SEC. CON "for", coincide con la anterior
size(id2); areEqual(id,id2);

// HE PARADO LOS CALCULOS CON "gz" Y CON "ad", NO HE PROBADO LOS OTROS
//
//

list pdg=primdecGTZ(i);  // 39 SEC. CON EL METODO GENERAL DE DESCOMP. PRIMARIA
size(pdg);

//
//
// EX 13:
ring s8=0,(x(0..5),s,t),dp;
ideal i=x(0)-st24,x(1)-s2t23,x(2)-s3t22,x(3)-s9t16,x(4)-s11t14,x(5)-s18t7;
ideal ei=eliminate(i,st);
ring s6=0,x(0..5),dp;
ideal I=imap(s8,ei);
ideal i=simplify(lead(std(I)),1);
isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);

list id=irreddecMon(i);

list id1=irreddecMon(i,"vas");

list id2=irreddecMon(i,"gz");

list id3=irreddecMon(i,"ad");

list id4=irreddecMon(i,"for");

list id5=irreddecMon(i,"mil");

list id6=irreddecMon(i,"lr");

list id8=irreddecMon(i,"sr");

size(pd);size(id);size(id1);size(id2);size(id3);size(id4);size(id5);size(id6);size(id8);

areEqual(id,id2);areEqual(id,id1);areEqual(id,id3);areEqual(id,id4);areEqual(id,id5);areEqual(id,id6);areEqual(id,id8);

//
//
// EX 14:
ring s15=0,(s,t,u,x(0..11)),dp;
ideal i=x(0)-s3t6u6,x(1)-s5t5u5,x(2)-t9s6,x(3)-st4u10,x(4)-s2t12u,x(5)-t14u,
          x(6)-s8t7,x(7)-s4t6u5,x(8)-t13su,x(9)-s4t9u2,x(10)-stu13,x(11)-s5t7u3;
ideal ei=eliminate(i,stu);
ring s12=0,x(0..11),dp;
ideal I=imap(s15,ei);
ideal i=simplify(lead(std(I)),1);
size(i);

isMonomial(i);
isprimeMon(i);
isprimaryMon(i);
isirreducibleMon(i);
isartinianMon(i);
isgenericMon(i);

list pd=primdecMon(i);  size(pd); // 1 SEGUNDOS CON "add", 9 componentes
;

list id=irreddecMon(i);  size(id); // 0 SEGUNDO CON "add", 112 componentes
;

list id1=irreddecMon(i,"sr");  // 46 SEC. CON "sr", coincide con la anterior
size(id1); areEqual(id,id1);

list id2=irreddecMon(i,"for");   //7 SEC. CON "for", coincide con la anterior
size(id2); areEqual(id,id2);

//
//
// EX 15:
setring s15;
i=x(0)-st6u8,x(1)-s5t3u7,x(2)-t11u4,x(3)-s9t4u2,x(4)-s2t7u6,x(5)-s7t7u,
          x(6)-s10t5,x(7)-s4t6u5,x(8)-s13tu,x(9)-s14u,x(10)-st2u12,x(11)-s3t9u3;
ei=eliminate(i,stu);
setring s12;
ideal II=imap(s15,ei);
ideal ii=simplify(lead(std(II)),1);
size(ii);

isMonomial(ii);
isprimeMon(ii);
isprimaryMon(ii);
isirreducibleMon(ii);
isartinianMon(ii);
isgenericMon(ii);

list ppdd=primdecMon(ii);  size(ppdd); // 0 SEGUNDOS CON "add", 10 componentes
;

list iidd=irreddecMon(ii);  size(iidd); // 1 SEGUNDO CON "add", 111 componentes
;

list pdg=primdecGTZ(ii);  // 9 SEC. CON EL METODO GENERAL DE DESCOMP. PRIMARIA
size(pdg);

// COMPARAMOS AHORA LOS RADICALES DE LAS COMPONENTES PRIMARIAS OBTENIDAS POR EL
// METODO GENERAL (rad2) Y POR EL METODO ESPECIFICO add (rad1)
list rad1=radicalMon(ppdd[1]),radicalMon(ppdd[2]),radicalMon(ppdd[3]),radicalMon(ppdd[4]),radicalMon(ppdd[5]),
         radicalMon(ppdd[6]),radicalMon(ppdd[7]),radicalMon(ppdd[8]),radicalMon(ppdd[9]),radicalMon(ppdd[10]);
list rad2=pdg[1][2],pdg[2][2],pdg[3][2],pdg[4][2],pdg[5][2],pdg[6][2],pdg[7][2],pdg[8][2],pdg[9][2],pdg[10][2];

areEqual(rad1,rad2);


tst_status(1);$
