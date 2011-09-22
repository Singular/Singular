LIB "tst.lib"; tst_init();
  intvec opt = option(get);
option(prot);
ring r  = 0,(a,b,c,d),dp;
ideal i = a+b+c+d,ab+ad+bc+cd,abc+abd+acd+bcd,abcd-1;
groebner(i);
ring s  = 0,(a,b,c,d),lp;
ideal i = imap(r,i);
groebner(i,"hilb");
ring R  = (0,a),(b,c,d),lp;
minpoly = a2+1;
ideal i = a+b+c+d,ab+ad+bc+cd,abc+abd+acd+bcd,d2-c2b2;
groebner(i,"par2var","slimgb");
groebner(i,"fglm");          //computes a reduced standard basis
option(set,opt);
tst_status(1);$
