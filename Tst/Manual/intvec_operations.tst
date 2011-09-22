LIB "tst.lib"; tst_init();
  intvec iv =  1,3,5,7,8;
  iv+1;               // add 1 to each entry
  iv*2;
  iv;
  iv-10;
  iv=iv,0;
  iv;
  iv div 2;
  iv+iv;              // component-wise addition
  iv[size(iv)-1];     // last-1 entry
  intvec iw=2,3,4,0;
  iv==iw;             // lexicographic comparison
  iv < iw;
  iv != iw;
  iv[2];
  iw = 4,1,2;
  iv[iw];
tst_status(1);$
