LIB "tst.lib"; tst_init();
LIB "control.lib";
// TwoPendula
ring r=(0,m1,m2,M,g,L1,L2),Dt,dp;
module RR =
[m1*L1*Dt^2, m2*L2*Dt^2, -1, (M+m1+m2)*Dt^2],
[m1*L1^2*Dt^2-m1*L1*g, 0, 0, m1*L1*Dt^2],
[0, m2*L2^2*Dt^2-m2*L2*g, 0, m2*L2*Dt^2];
module R = transpose(RR);
module SR = std(R);
matrix T = lift(R,SR);
genericity(T);
//-- The result might be different when computing reduced bases:
matrix T2;
option(redSB);
option(redTail);
module SR2 = std(R);
T2 =  lift(R,SR2);
genericity(T2);
tst_status(1);$
