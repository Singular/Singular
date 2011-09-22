LIB "tst.lib"; tst_init();
  ring R= 0,(u,v,x,y,z),dp;
  ideal I = ux, vx, uy, vy;
  resolution resI = mres(I,0); resI;
  // the betti number:
  print(betti(resI), "betti");
  // the regularity:
  regularity(resI);
tst_status(1);$
