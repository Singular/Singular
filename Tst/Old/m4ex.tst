  ring Rt = (0,t),(x,y),lp;
  Rt;
  poly f = x5+y11+xy9+x3y9;
  ideal i = jacob(f);
  ideal j	= i,i[1]*i[2]+t*x5y8;  // deformed ideal
  j;
  vdim(std(j));
  kill Rt;
$
