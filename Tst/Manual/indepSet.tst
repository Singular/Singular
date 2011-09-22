LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,u,v,w),dp;
  ideal I=xyw,yvw,uyw,xv;
  attrib(I,"isSB",1);
  indepSet(I);
  eliminate(I,vw);
  indepSet(I,0);
  indepSet(I,1);
  eliminate(I,xuv);
tst_status(1);$
