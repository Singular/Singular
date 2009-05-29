LIB "tst.lib";
tst_init();
//option(prot);
"========= Alex4";
ring h4=32003,(t,x,y,z,w),(dp,c);
ideal j=
4t4z3y+6z4x3t5+5x5z7,
5t6z3y3x+5x2z4t3y7+4x5t5y,
6z7t5y+6z3y2t4+2t8z6;
ideal i=homog(j,w);
timer=1;
list I=mres(i,0);
betti(I);
kill h4;

"======== standard";
ring rh2=32003,(w,x,y,z),(dp,c);
poly p1 = 1x3y2+21328x5y+10667x2y4+21328x2yz3+10666xy6+10667y9;
poly p2 = 1x2y2z2+3z8;
poly p3 = 5x4y2+4xy5+2x2y2z3+1y7+11x10;
ideal j= p1,p2,p3;
ideal i=homog(j,w);
timer=1;
list I=mres(i,0);
betti(I);
kill rh2;

"== f(11,10,3,1)";
ring rh3=32003,(w,x,y,z),(dp,c);
int a =11;
int b =10;
int c =3;
int t =1;
poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j= jacob(f);
ideal i=homog(j,w);
timer=1;
list I=mres(i,0);
betti(I);
kill b,c,t,a,rh3;

"======== h(6)";
ring rh4=32003,(w,x,y,z),(dp,c);
int a=6;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal j= jacob(f);
ideal i=homog(j,w);
timer=1;
list I=mres(i,0);
betti(I);
kill a,rh4;

"======== h(7)";
ring rh5=32003,(w,x,y,z),(dp,c);
int a =7;
poly f=xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal j= jacob(f);
ideal i=homog(j,w);
timer=1;
list I=mres(i,0);
betti(I);
kill a,rh5;

"======== g(6,8,10,5,5;0)";
ring rh7=32003,(w,x,y,z),(dp,c);
int a= 6;
int b= 8;
int c =10;
int alpha=5;
int beta= 5;
int t=0;
poly f =x^a+y^b+z^c+x^alpha*y^(beta-5)+x^(alpha-2)*y^(beta-3)+x^(alpha-3)*y^(beta-4)*z^2+x^(alpha-4)*y^(beta-4)*(y^2+t*x)^2;
ideal j =jacob(f); 
ideal i=homog(j,w);
timer=1;
list I=mres(i,0);
betti(I);
kill a, b, c, alpha ,beta, t, rh7;

"======== (max5)^2";
ring so2=32003,(h,w,x,y,z,t),(dp,c);
ideal in =wx2y2z2t2+w,w2xy2z2t2+x,w2x2yz2t2+y,w2x2y2zt2+z,w2x2y2z2t+t;
ideal i = homog(in,h)^2;
timer=1;
list I=mres(i,0);
betti(I);
kill so2;

"==randomSyz1";
ring ra=32003,(a,b,c,d,e,f),(dp,c);
ideal i=
6739a3+1039a2b-7486ab2+3718b3-11069a2c+6473abc-5891b2c-12382ac2+1623bc2
+4363c3-5151a2d+15452abd+13118b2d-7061acd+4295bcd+204c2d-12178ad2+7928bd2+14cd2
+5562d3-6285a2e-10789abe+1785b2e+687ace+11524bce+12733c2e+8483ade-11294bde-3594cde
+12201d2e-7682ae2+3312be2-268ce2-13575de2+5144e3,
4250a3-5177a2b+13404ab2+426b3+11302a2c-15885abc+12078b2c-1036ac2-1799bc2
-7475c3-9907a2d-6162abd+10416b2d+2042acd+15283bcd+11998c2d+575ad2+719bd2-7042cd2
+4028d3+8193a2e-9701abe-3251b2e+12458ace-10367bce+3348c2e+10191ade+13321bde
+13623cde-5183d2e+9584ae2-10484be2-6235ce2-15629de2+2327e3,
6550a3+4965a2b+12096ab2-12270b3-14859a2c+2541abc-8313b2c-13376ac2-2239bc2
+2634c3-1804a2d-9400abd-15441b2d+4784acd+7087bcd-10294c2d+8698ad2-6351bd2-387cd2
-11534d3-13666a2e+14767abe+5618b2e+8275ace+675bce+11539c2e-3124ade-14915bde
+15670cde-7223d2e-1840ae2-14459be2+895ce2-12702de2+2068e3, 
15942a3+4739a2b-5019ab2-6691b3+600a2c-1561abc+14937b2c-14659ac2-8392bc2
+12410c3+3327a2d-14426abd+7985b2d-10298acd-2630bcd-4528c2d-12983ad2+8567bd2
-14036cd2+10885d3-7763a2e-3366abe-3579b2e+2908ace-5392bce-336c2e+10534ade-11990bde
+12114cde-4105d2e+14357ae2-3578be2-5592ce2+13057de2+10409e3;
timer=1;
list I=mres(i,0);
betti(I);
kill ra;
tst_status(1);$
