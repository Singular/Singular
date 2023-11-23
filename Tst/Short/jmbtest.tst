LIB "tst.lib";
tst_init();

//EXAMPLES SHEET
LIB"JMBTest.lib";

//Example 1

ring r=0, (x,y,z),ip;
jmp r1;
r1.h=z^5;
r1.t=poly(0);
jmp r2;
r2.h=z^4*y;
r2.t=poly(0);
jmp r3;
r3.h=z^3*y^2;
r3.t= poly(0) ;
jmp r4;
r4.h=z^2*y^3;
r4.t= poly(0) ;
jmp r5;
r5.h=z^4*x;
r5.t= poly(0) ;
jmp r6;
r6.h=z^3*y*x  ;
r6.t= poly(0) ;
jmp r7;
r7.h=  z^2*y^2*x;
r7.t= poly(0) ;
jmp r8;
r8.h= z*y^4 ;
r8.t= poly(0) ;
jmp r9;
r9.h=z*y^3*x  ;
r9.t= poly(0) ;
jmp r10;
r10.h= z^3*x^2 ;
r10.t=poly(0)  ;
jmp r11;
r11.h= z^2*y*x^2 ;
r11.t=poly(0)  ;
jmp r12;
r12.h=z*y^2*x^2;
r12.t=-y^4*x-z^2*x^3;
jmp r13;
r13.h= y^5;
r13.t=poly(0);
list G1V=list(list(r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13));
TestJMark(G1V,r);
//Result 1
//=>1



//Example 2
ring r=0, (x,y,z),ip;
jmp r1;
r1.h=z^3;
r1.t=poly(0);
jmp r2;
r2.h=z^2*y;
r2.t=poly(0);
jmp r3;
r3.h=z*y^2 ;
r3.t=-x^2*y;
jmp r4;
r4.h=y^5;
r4.t=poly(0);
list G2F=list(list(r1,r2,r3),list(r4));
TestJMark(G2F,r);
//Result 2
//=> NOT A BASIS
//0



//Example 3
ring r=0, (x(0..3)), ip;
jmp r1;
r1.h=x(3)^2;
r1.t=-4*x(0)*x(2)+x(1)^2;
jmp r2;
r2.h=x(3)*x(2);
r2.t=-x(1)*x(0);
jmp r3;
r3.h=x(2)^2;
r3.t=-5*x(0)*x(2);
jmp r4;
r4.h=x(3)*x(1);
r4.t=poly(0) ;
jmp r5;
r5.h=x(2)*x(1);
r5.t=-x(0)^2;
jmp r6;
r6.h=x(1)^3;
r6.t=-x(0)*x(1)^2;
list G3F=list(list(r1, r2, r3, r4, r5),list(r6));
TestJMark(G3F,r);
//Result 3
//=> NOT A BASIS
//0



//Example 4
ring r=31991, (x(0..3)), ip;
jmp r1;
r1.h=x(3)^2;
r1.t=11415*x(1)*x(2) + 12545*x(0)*x(2) + 5551*x(1)^2 + 2052*x(0)*x(1) - 5996*x(0)^2;
jmp r2;
r2.h=x(2)*x(3);
r2.t= 9738*x(1)*x(2) + 4702*x(0)*x(2) + 1299*x(1)^2 - 9258*x(0)*x(1) - 660*x(0)^2;
jmp r3;
r3.h=x(1)*x(3);
r3.t= - 499*x(1)*x(2) + 15889*x(0)*x(2) - 7174*x(1)^2 + 14455*x(0)*x(1) + 9142*x(0)^2;
jmp r4;
r4.h=x(0)*x(3);
r4.t= - 7284*x(1)*x(2) - 252*x(0)*x(2) + 14903*x(1)^2 + 3168*x(0)*x(1) - 7681*x(0)^2;
jmp r5;
r5.h=x(2)^2;
r5.t= - 966*x(1)*x(2) - 3371*x(0)*x(2) - 7034*x(1)^2 + 13657*x(0)*x(1) - 7337*x(0)^2;
jmp r6;
r6.h=x(0)*x(1)*x(2);
r6.t= 15317*x(0)^2*x(2) - 15115*x(1)^3 + 7262*x(0)*x(1)^2 - 5022*x(0)^2*x(1) + 9052*x(0)^3;
jmp r7;
r7.h= x(1)^2*x(2);
r7.t=- 2219*x(0)^2*x(2) + 7022*x(1)^3 - 8908*x(0)*x(1)^2 - 14283*x(0)^2*x(1) + 6536*x(0)^3 ;
jmp r8;
r8.h=x(0)^3*x(2);
r8.t=570*x(1)^4 - 5548*x(0)*x(1)^3 - 5727*x(0)^2*x(1)^2 - 12284*x(0)^3*x(1) + 5207*x(0)^4 ;
jmp r9;
r9.h=x(1)^5;
r9.t= -14551*x(0)*x(1)^4+11335*x(0)^2*x(1)^3+11357*x(0)^3*x(1)^2-5733*x(0)^4*x(1)-3043*x(0)^5;
list G4V=list(list(r1, r2, r3,  r4, r5), list(r6,r7),list(r8),list(r9));
TestJMark(G4V,r);
//Result 4
//=>1



//Example 5
ring r=0, (x,y,z),ip;
jmp r1;
r1.h=xy;
r1.t=poly(0);
jmp r2;
r2.h=y^2;
r2.t=poly(0);
jmp r3;
r3.h=xz;
r3.t= poly(0) ;
jmp r4;
r4.h=yz;
r4.t= poly(0) ;
jmp r5;
r5.h=z^2;
r5.t= poly(0) ;
list G5V=list(list(r1,r2,r3,r4,r5));
TestJMark(G5V,r);
//Result 5
//=>1



//Example 6
ring r=0, (x,y,z),ip;
jmp r1;
r1.h=xy;
r1.t=poly(0);
jmp r2;
r2.h=y^2;
r2.t=3*x^2;
jmp r3;
r3.h=xz;
r3.t= poly(0) ;
jmp r4;
r4.h=yz;
r4.t= poly(0) ;
jmp r5;
r5.h=z^2;
r5.t= poly(0) ;
list G6F=list(list(r1,r2,r3,r4,r5));
TestJMark(G6F,r);
//Result 6
//=> NOT A BASIS
//0



//Example 7
ring r=0, (z,y,x),ip;
jmp r1;
r1.h=x^4;
r1.t=poly(0);
jmp r2;
r2.h=x^3*y;
r2.t=poly(0);
jmp r3;
r3.h=x^2*y^2;
r3.t= poly(0) ;
jmp r4;
r4.h=x*y^3;
r4.t= poly(0) ;
jmp r5;
r5.h=z*x^3;
r5.t= poly(0) ;
jmp r6;
r6.h=x^2*y*z ;
r6.t= y^4 ;
jmp r7;
r7.h=  x*y^2*z;
r7.t= poly(0) ;
jmp r8;
r8.h= y^5 ;
r8.t= poly(0) ;
list G7V=list(list(r1,r2,r3,r4,r5,r6,r7),list(r8));
TestJMark(G7V,r);

//Result 7
//=>1



//Example 8
ring r=0, (z,y,x),ip;
jmp r1;
r1.h=x^4;
r1.t=poly(0);
jmp r2;
r2.h=x^3*y;
r2.t=poly(0);
jmp r3;
r3.h=x^2*y^2;
r3.t= poly(0) ;
jmp r4;
r4.h=x*y^3;
r4.t= y^4 ;
jmp r5;
r5.h=z*x^3;
r5.t= poly(0) ;
jmp r6;
r6.h=x^2*y*z ;
r6.t= poly(0) ;
jmp r7;
r7.h=  x*y^2*z;
r7.t= z*y^3 ;
jmp r8;
r8.h= y^5 ;
r8.t= poly(0) ;
list G8F=list(list(r1,r2,r3,r4,r5,r6,r7),list(r8));
TestJMark(G8F,r);
//Result 8
//=> NOT A BASIS
//0



//Example 9
ring r=0, (x,y,z),ip;
jmp r1;
r1.h=z;
r1.t=poly(0);
jmp r2;
r2.h=y^6;
r2.t=poly(0);
jmp r3;
r3.h=x^3*y^5;
r3.t=poly(0);
list G9V=list( list(r1),list(),list(),list(),list(),list(r2),list(),list(r3));
TestJMark(G9V,r);
//Result 9
//=>1




//Example 10
ring r=0, (x,y,z), ip;
jmp r1;
r1.h=z;
r1.t=poly(0);
jmp r2;
r2.h=y^6;
r2.t=poly(0);
jmp r3;
r3.h=x^3*y^5;
r3.t=x^8;
list G10F=list( list(r1),list(),list(),list(),list(),list(r2),list(),list(r3));
TestJMark(G10F,r);
//Result 10
//=> NOT A BASIS
//0



//Example 11
ring r=0, (x,y,z), ip;
jmp r1;
r1.h=z;
r1.t=poly(0);
jmp r2;
r2.h=y^6;
r2.t=x*y^5;
jmp r3;
r3.h=x^3*y^5;
r3.t=poly(0);
list G11V=list( list(r1),list(),list(),list(),list(),list(r2),list(),list(r3));
TestJMark(G11V,r);
//Result 11
//=>1



//Example 12
ring r=0, (x,y,z), ip;
jmp r1;
r1.h=z;
r1.t=poly(0);
jmp r2;
r2.h=y^6;
r2.t=x*y^5+x^2*y^4+x^3*y^3+x^4*y^2+x^5*y;
jmp r3;
r3.h=x^5*y^3+x^7*y;
r3.t=poly(0);
list G12V=list( list(r1),list(),list(),list(),list(),list(r2),list(),list(r3));
TestJMark(G12V,r);
//Result 12
//=>1

//Example 13
ring r=0, (x,y,z), ip;
jmp r1;
r1.h=z;
r1.t=x+y;
jmp r2;
r2.h=y^6;
r2.t=x*y^5+x^2*y^4+x^3*y^3+x^4*y^2+x^5*y;
jmp r3;
r3.h=x^5*y^3+x^7*y;
r3.t=poly(0);
list G13F=list( list(r1),list(),list(),list(),list(),list(r2),list(),list(r3));
TestJMark(G13F,r);
//Result 13
//=>NOT A BASIS
//0

//Example 14
ring r=0, (x,y,z),ip;
jmp r1;
r1.h=yz;
r1.t=y^2;
jmp r2;
r2.h=z^2;
r2.t=xy+xz+y^2;
jmp r3;
r3.h=x*y^2;
r3.t= poly(0) ;
jmp r4;
r4.h=y^3;
r4.t=poly(0);
jmp r5;
r5.h=x^4;
r5.t= poly(0) ;
jmp r6;
r6.h=x^3*y  ;
r6.t= poly(0) ;
jmp r7;
r7.h=  x^3*z;
r7.t= poly(0) ;
list G14V=list(list(r1,r2),list(r3,r4),list(r5,r6,r7));
TestJMark(G14V,r);
//Result 14
//=>1



//Example 15
ring r=0, (x,y,z),ip;
jmp r1;
r1.h=yz;
r1.t=y^2;
jmp r2;
r2.h=z^2;
r2.t=xy+xz+y^2;
jmp r3;
r3.h=x*y^2;
r3.t= poly(0) ;
jmp r4;
r4.h=y^3;
r4.t=x^3;
jmp r5;
r5.h=x^4;
r5.t= poly(0) ;
jmp r6;
r6.h=x^3*y  ;
r6.t= poly(0) ;
jmp r7;
r7.h= x^3*z;
r7.t= poly(0) ;
list G15F=list(list(r1,r2),list(r3,r4),list(r5,r6,r7));
TestJMark(G15F,r);
//Result 15
//=>NOT A BASIS

tst_status(1);$
