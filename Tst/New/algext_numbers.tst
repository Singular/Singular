LIB "tst.lib"; tst_init();

ring r=(0,Q),(x,y,z),dp; basering;

minpoly = Q2-Q+1; basering;

minpoly;

number qm2 = 1/Q2; // -Q?
qm2;
qm2 == (-Q);



number q2 = Q2; // Q-1!
q2;
q2 == (Q-1); // BUG!


q2 * qm2; // 1?
qm2 * q2; // 1?

tst_status(1);$
