  ring r =32003,(a,b,c,d,e),lp;
  ideal i=a+b+c+d, ab+bc+cd+ae+de, abc+bcd+abe+ade+cde,
          abc+abce+abde+acde+bcde, abcde-1;
  option(prot);
  ideal j1=stdfglm(i); 
  size(j1);   // size (no. of polys) in computed GB
  ideal j2=stdhilb(i);
  size(j2);   // size (no. of polys) in computed GB
  // usual Groebner basis computation for lex ordering
  ideal j0 =std(i);
  size(j0);
  ideal j = groebner(i);
  size(j); 
LIB "tst.lib";tst_status(1);$
