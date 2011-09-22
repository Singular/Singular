LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
module m=[1,y],[0,x+z];
intmat M=betti(mres(m,0));
list l=r,m,matrix(M);
printf("s:%s,l:%l",1,2);
printf("s:%s",l);
printf("s:%s",list(l));
printf("2l:%2l",list(l));
printf("%p",matrix(M));
printf("%;",matrix(M));
printf("%b",M);
tst_status(1);$
