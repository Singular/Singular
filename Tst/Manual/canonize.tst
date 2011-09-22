LIB "tst.lib"; tst_init();
LIB "control.lib";
// TwoPendula with L1=L2=L
ring r=(0,m1,m2,M,g,L),Dt,dp;
module RR =
[m1*L*Dt^2, m2*L*Dt^2, -1, (M+m1+m2)*Dt^2],
[m1*L^2*Dt^2-m1*L*g, 0, 0, m1*L*Dt^2],
[0, m2*L^2*Dt^2-m2*L*g, 0, m2*L*Dt^2];
module R = transpose(RR);
list C = control(R);
list CC = canonize(C);
view(CC);
tst_status(1);$
