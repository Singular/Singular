  ring r =32003,(a,b,c,d,e),lp;
  ideal i=a+b+c+d, ab+bc+cd+ae+de, abc+bcd+abe+ade+cde,
          abc+abce+abde+acde+bcde, abcde-1;
  int t=timer;
  ideal j1=stdfglm(i); 
  timer-t;    
  size(j1);   // size (no. of polys) in computed GB
  t=timer;
  ideal j2=stdhilbert(i);
  timer-t;    
  size(j2);   // size (no. of polys) in computed GB
  // usual Groebner basis computation for lex ordering
  t=timer;
  ideal j0 =std(i); 
  timer-t;    
LIB "tst.lib";tst_status(1);$
