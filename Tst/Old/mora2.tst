" ============= cyclic_roots_7(homog) d- ==========================";
"deg 60, dim 2, elem 99, Ende im Grad 17
//Macaulay 3.55 min (SE/30), 2.45 sec (PowerBook)
//Sing1/93 (test 4 12) 3.01 min SE/30, 2.45 (PowerBook)";
ring r6 = 32003,(a,b,c,d,e,f,g),ds;
r6;
poly s1=a+b+c+d+e+f;
poly s2=ab+bc+cd+de+ef+fa;
poly s3=abc+bcd+cde+edf+efa+fab;
poly s4=abcd+bcde+cdef+defa+efab+fabc;
poly s5=abcde+bcdef+cdefa+defab+efabc+fabcd;
poly s6=abcdef+g^6;
ideal i=s1,s2,s3,s4,s5,s6;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;
kill r6;
" ============= standard(homog) l1 d3- ==========================";
"deg 720, dim 1, elem 68
// Sing1/93c:39sec (4 12)  (PowerBook)";
ring r8 = 32003,(t,x,y,z),(ls(1),ds(3));
r8;
poly s1=1x3y2t4+21328x5yt3+10667x2y4t3+21328x2yz3t3+10666xy6t2+10667y9;
poly s2=1x2y2z2t2+3z8;
poly s3=5x4y2t4+4xy5t4+2x2y2z3t3+1y7t3+11x10;
ideal i=s1,s2,s3;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;
kill r8;
" ============= 3 Puiseux-Paare ==========================";
"deg , dim, elem ";
listvar(all);
ring p2=32003,(t, x, y),ls;
poly s1=x-t72;
poly s2=y-t84-t111-t160;
ideal i=s1,s2;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
j;                                                        
kill p2;
$
