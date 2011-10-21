LIB "tst.lib";
tst_init();

// error from idLift not testet for
ring r=(16,a),(e2,e1,y(2..1)),(wp(1+30,1,1,1));
qring rq=std(ideal(e1^2, e2^2, e1*e2));
division(e2, std(0));


tst_status(1);$
