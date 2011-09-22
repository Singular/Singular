LIB "tst.lib"; tst_init();
  ring R= 0,(u,v,x,y,z),dp;
  ideal i=ux, vx, uy, vy;
  print(syz(i));
tst_status(1);$
