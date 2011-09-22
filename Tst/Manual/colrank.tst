LIB "tst.lib"; tst_init();
LIB "control.lib";
// de Rham complex
ring r=0,(D(1..3)),dp;
module R;
R=[0,-D(3),D(2)],
[D(3),0,-D(1)],
[-D(2),D(1),0];
R=transpose(R);
colrank(R);
tst_status(1);$
