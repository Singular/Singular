LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
module m=[1,y],[0,x+z];
intmat M=betti(mres(m,0));
list l=r,m,M;
link li="";   // link to stdout
fprintf(li,"s:%s,l:%l",1,2);
fprintf(li,"s:%s",l);
fprintf(li,"s:%s",list(l));
fprintf(li,"2l:%2l",list(l));
fprintf(li,"%p",list(l));
fprintf(li,"%;",list(l));
fprintf(li,"%b",M);
tst_status(1);$
