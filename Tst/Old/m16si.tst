  intvec iv=1,3,5,7,8;
  iv=iv,iv;           // concatenation
  iv+1;               // add 1 to each entry
  iv*2;
  iv;
  iv-10;
  iv=iv,0;
  iv;
  iv/2;
  iv+iv;              // componentwise addition
  iv[size(iv)-1];     // last-1 entry
  intvec iw=2,3,4,0;
  iv==iw;             // comparision
  intmat im[2][4];
  im=2,3,4,0,1,0,0,0;
  iv=im*iw;
  iv;
$
