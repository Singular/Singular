  ring r=0,(x,y,z),dp;
  ideal i=x,y+z,2xz;
  ring R=0,(a,b,x,y,c),lp;
  ideal i=imap(r,i);
  i;
$
