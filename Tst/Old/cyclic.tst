option(prot,sugarCrit);
" ============= cyclic_roots_5(isol) + ==========================";
ring r4 = 0,(a,b,c,d,e),dp;
r4;
poly s1=a+b+c+d+e;
poly s2=de+1cd+1bc+1ae+1ab;
poly s3=cde+1bcd+1ade+1abe+1abc;
poly s4=bcde+1acde+1abde+1abce+1abcd;
poly s5=abcde+1;
ideal i=s1,s2,s3,s4,s5;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
kill r4;
"====================  standard char0  =============================";
//      H7 l, char 0, test0,11,1: 61/31 ohne vollst; Reduktion
ring r=
0,(x,y),lp;
poly f=x5+y11+xy9+x3y9;
ideal i=jacob(f);
ideal j=std( i);
size(j);
kill r;
" ============= cyclic_roots_6(homog) dp ==========================";
ring r6 = 0,(a,b,c,d,e,f),dp;
poly s1=a+b+c+d+e+f;
poly s2=ab+bc+cd+de+ef+fa;
poly s3=abc+bcd+cde+edf+efa+fab;
poly s4=abcd+bcde+cdef+defa+efab+fabc;
poly s5=abcde+bcdef+cdefa+defab+efabc+fabcd;
poly s6=abcdef+1;
ideal i=s1,s2,s3,s4,s5,s6;
ideal j=std(i);
"dim: "+ string(dim(j)) +",  mult: "+ string(mult(j)) +",  elem: "
                                                        + string(size(j));
kill r6;
LIB "tst.lib";tst_status(1);$
