//Testserie fuer std im Mora-Fall
"              Testbeispiele Mora ";
//timer=1;
//test(0);
" ";
" ============= standard d - ==========================";
"elem 19, dim 0, mult 312 ";
";Sing 1.93 (test 3 4 15):4s, (test 3 10 15):6s; Quadra (test4 15): 1 sec";
ring r1 = 32003,(z,y,x),ds;
r1;
poly s1=1x3y2+21328x5y+10667x2y4+21328x2yz3+10666xy6+10667y9;
poly s2=1x2y2z2+3z8;
poly s3=5x4y2+4xy5+2x2y2z3+1y7+11x10;
ideal i=s1,s2,s3;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                + string(size(j));
j;
kill r1;
" ============= (19,19,4) d - ==========================";
"elem 28, dim 0, mult1040
//Sing1/93, (10,15):60s, Quadra (10,15): 22s";
ring r2 = 32003,(x,y,z),ds;
r2;
int a=19; int b=19; int c=4; int t=1;
"a b c t = ",a,b,c,t;
poly f=x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
f;
ideal i=diff(f,x),diff(f,y),diff(f,z);
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;
kill a,b,c,t,r2;
" =========== (a,b,c,d,e,t) min=(9,9,15,5,6) d - =================";
"elem 17, dim 0, mult 154 ?
//Sing1/93, (test 10,15):60s, Quadra (test 10,15): 22s";
ring r3 = 32003,(x,y,z),ds;
r3;
int a=9; int b=9; int c=13; int d=5; int e=5; int t=1;
"a b c d e t = ",a,b,c,d,e,t;
poly
f=x^a+y^b+z^c+x^d*y^(e-5)+x^(d-2)*y^(e-3)+x^(d-3)*y^(e-4)*z^2+x^(d-4)*y^(e-4)*(y^2+t*x)^2;
f;
ideal i=diff(f,x),diff(f,y),diff(f,z);
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;
kill a,b,c,e,d,t,r3;
" ============= cyclic_roots_5(isol) + ==========================";
"deg 131, dim 0, elem 30, Ende im Grad 12
//Macaulay 20 sec (SE/30), Quadra (4 10 15):5 sec";
//Sing1/93:19 sec (test 4 12) (SE/30), 12sec (PowerBook)
ring r4 = 32003,(a,b,c,d,e),ds;
r4;
int n=10;
poly s1=a+b+c+d+e;
poly s2=de+1cd+1bc+1ae+1ab;
poly s3=cde+1bcd+1ade+1abe+1abc;
poly s4=bcde+1acde+1abde+1abce+1abcd;
poly s5=abcde;
ideal i=s1,s2,s3,s4,s5,a^n,b^n,c^n,d^n,e^n;
i;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;
kill r4,n;
" ============= cyclic_roots_6(homog) d- ==========================";
"deg 120, dim 1, elem 38, Ende im Grad 14
//Macaulay 17 sec (SE/30), 13 sec (PowerBook)
//Sing1/93 10sec (test 4 12) SE/30, 7sec (PowerBook)";
ring r5 = 32003,(a,b,c,d,e,f),ds;
r5;
poly s1=a+b+c+d+e;
poly s2=de+1cd+1bc+1ae+1ab;
poly s3=cde+1bcd+1ade+1abe+1abc;
poly s4=bcde+1acde+1abde+1abce+1abcd;
poly s5=f^5+1abcde;
ideal i=s1,s2,s3,s4,s5;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;
kill r5;
" ============= standardzyx(homog) l1 d3- ==========================";
"deg 720, dim 1, elem 109, Ende im Grad 53
//Macaulay 11;48 min (SE/30)
//Sing1/93c: 9;58min (test 4 12) (SE/30)";
ring r7 = 32003,(t,z,y,x),(ls(1),ds(3));
r7;
poly s1=1x3y2t4+21328x5yt3+10667x2y4t3+21328x2yz3t3+10666xy6t2+10667y9;
poly s2=1x2y2z2t2+3z8;
poly s3=5x4y2t4+4xy5t4+2x2y2z3t3+1y7t3+11x10;
ideal i=s1,s2,s3;
i;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;
kill r7;
"=================  NF w:1 1 1  -  ==================";
//elem 5, dim 1, mult 4, Quadra (4 10 15):2 sec
//Sing1;93, (10,15):0s,##elem2##, (3,10,15):16s (Powerbook)
ring r10=32003,(x,y,z),(ws(1,1,1));
poly s1=x2-y2;
poly s2=y3;
poly s3=y2-zyx;
poly s4=xy2z;
ideal i=s1,s3,s4;
ideal j=std(i);
//size(j);
degree (j);
reduce(s2,j);
kill r10;
"====================  parametric curves l -   ==================";
//Zeit: (l,test 0) H7 0/59, W 26/53
ring a1=32003,(x,y,z,t),ls;
ideal i=
x31-x6-x-y,
x8-z,
x10-t;
ideal j=std (i);
//size(j);
degree (j);
j;
kill a1;
"====================  standard char0  =============================";
//      H7 l, char 0, test0,11,1: 61/31 ohne vollst; Reduktion
ring r=
0,(x,y),ls;
poly f=x5+y11+xy9+x3y9;
ideal i=jacob(f);
ideal j=std( i);
j;
degree (j);
//size(j);
kill r;
"=============== integer-programming2  l1 d8 - =================";
//elem 26, dim 6, mult 4191
//Sing 1;93 (4 15):2s, (3 4 15):2s (SE/30)
ring a3=32003,(t,a,b,c,d,e,x,y,z),(ls(1),ds(8));
ideal i=
-y82a+x32z23,
x45-y13z21b,
y33z12-z41c,
-y33z12d+x22,
x5y17z22e,
xyzt;
i;
ideal j=std (i);
j;
degree (j);
//size(j);
kill a3;
"================ entartet d- =================";
ring r=32003,(x,y,z),ds;
int s=1;
int t=1;
int u=1;
poly f=(xyz+s*xy+t*yz+u*xz)*(x+y+z)^2 +x12+y12+z12;
ideal i=jacob(f);
ideal j=std(i);
degree(j);
j;
kill r;
"====================  omega2 d -   ==================";
ring M=32003,(x,y,z),ds;
int o=167;
int m=167;
poly f1=xy+z^(o-1);
poly f2=xz+y^(m-1)+yz2;
poly fx=diff(f1,x);
poly fy=diff(f1,y);
poly fz=diff(f1,z);
poly gx=diff(f2,x);
poly gy=diff(f2,y);
poly gz=diff(f2,z);
module i=
[f1,0,0],
[0,f1,0],
[0,0,f1],
[f2,0,0],
[0,f2,0],
[0,0,f2],
[fy,fz,0],
[fx,0,-fz],
[0,fx,fy],
[gy,gz,0],
[gx,0,-gz],
[0,gx,gy];
module j=std (i);
j;
//size(j)
"dimension of omega 2 =";
degree(j);
$
