LIB "tst.lib";tst_init();

//gcd_univar_p_s.tst and gcd_univar_p_alpha_s.tst are derived from these computations

"untitled1(1/p/1)";
ring r=(32003,u),(t,x,y),dp;
ideal i=x-t15,y-t21-ut25+2t29-4t33+3t37+96/5t41-87t45;
option(prot);
ideal j=std(i);
size(j);
degree(j);

kill r;

"anborg 7(1/p/1)";
ring r=(32003,u),(x,y,z),dp;
ideal i=
u3x2+yz+xy2z+xyz2+xyz+xy+xz+yz, 
ux2y2z+xy2z2+x2yz+xyz+yz+x+z, 
u2x2y2z2+x2y2z+xy2z+xyz+xz+z+1;
option(prot);
option(redSB);
ideal j=std(i);
size(j);
degree(j);

ring s=(32003,u),(x,y,z),lp;
ideal k=fglm(r,j);

kill s;
kill r;

"coprasse(1/p/1)";
ring r=(32003,u),(x,y,z,t),dp; 
ideal i=
y2z+2xyt-2x-z,
-x3z+4xy2z+4x2yt+2y3t+4x2-10y2+4xz-10yt+2,
2uyzt+xt2-x-2z,
-xz3+4yz2t+4xzt2+2yt3+4xz+4z2-10yt-10t2+2;
option(prot);
option(redSB);
ideal j=std(i);
size(j);
degree(j);

ring s=(32003,u),(x,y,z,t),lp;
ideal k=fglm(r,j);

kill s;
kill r;

"inhomog cyclic 6(1/p/1)";
ring r=(32003,u),(a,b,c,d,e,f),dp;
ideal i=
a+b+c+d+e+f,
ab+bc+cd+de+ef+fa,
abc+bcd+cde+d*e*f + efa + fab,
uabcd+bcde+c*d*e*f+d*e*f*a + efab + fabc,
abcde+b*c*d*e*f+c*d*e*f*a+d*e*f*a*b+efabc+fabcd,
a*b*c*d*e*f;
option(prot);
ideal j=std(i);
size(j);
degree(j);


kill r;

"inhomog cyclic 5(1/p/1)";
ring r=(32003,u),(a,b,c,d,e),dp;
ideal i=
a+b+c+d+e,
ab+bc+cd+de+ea,
abc+bcd+cde+dea+eab,
uabcd+bcde+cdea+deab+eabc,
uabcde+1;
option(prot);
option(redSB);
ideal j=std(i);
size(j);
degree(j);


ring s=(32003,u),(a,b,c,d,e),lp;
ideal k=fglm(r,j);


kill s;
kill r;

"inhomog cyclic 5(1/p/2)";
ring r=(32003,u),(a,b,c,d,e),dp;
ideal i=
ua+b+c+d+e,
ab+bc+cd+de+ea,
abc+bcd+cde+dea+eab,
(u+1)*abcd+bcde+cdea+deab+eabc,
uabcde+1;
option(prot);
option(redSB);
ideal j=std(i);
size(j);
degree(j);


ring s=(32003,u),(a,b,c,d,e),lp;
ideal k=fglm(r,j);


kill s;
kill r;

"symmetric 4(1/p/1)";
ring r=(32003,u),(a,b,c,d),dp;
ideal i=
(1+u+u^2)*a4-b4,
b4-c4,
uc4-d4,
ua3b+b3c+c3d+d3a;
option(prot);
option(redSB);
ideal j=std(i);
size(j);
degree(j);


ring s=(32003,u),(a,b,c,d),lp;
ideal k=fglm(r,j);


kill s;
kill r;

tst_status(1);
$
