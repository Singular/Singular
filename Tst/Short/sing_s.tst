//sing_s.tst
//short tests for sing.lib
LIB "tst.lib";
tst_init();
LIB "sing.lib";
//example codim;     (liefert bei mir Unsinn, proc nicht von mir!)
example deform;
example dim_slocus;
example is_active;
example is_ci;
example is_is;
example is_reg;
example is_regs;
example milnor;
//example nf_icis;   (Fehler in proc sparsetriag von random.lib) 
example slocus;
example qhspectrum;
example Tjurina;
example tjurina;
example T_1;
example T_2;
example T_12;
tst_status(1);$
