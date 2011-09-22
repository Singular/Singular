LIB "tst.lib"; tst_init();
LIB "ntsolve.lib";
ring rsq = (real,40),(x,y,z,w),lp;
ideal gls =  x2+y2+z2-10, y2+z3+w-8, xy+yz+xz+w5 - 1,w3+y;
ideal ini = 3.1,2.9,1.1,0.5;
intvec ipar = 200,0;
ideal sol = nt_solve(gls,ini,ipar);
sol;
tst_status(1);$
