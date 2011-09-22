LIB "tst.lib"; tst_init();
LIB "control.lib";
//Example Antenna
ring r = (0, K1, K2, Te, Kp, Kc),(Dt, delta), (c,dp);
module RR;
RR =
[Dt, -K1, 0, 0, 0, 0, 0, 0, 0],
[0, Dt+K2/Te, 0, 0, 0, 0, -Kp/Te*delta, -Kc/Te*delta, -Kc/Te*delta],
[0, 0, Dt, -K1, 0, 0, 0, 0, 0],
[0, 0, 0, Dt+K2/Te, 0, 0, -Kc/Te*delta, -Kp/Te*delta, -Kc/Te*delta],
[0, 0, 0, 0, Dt, -K1, 0, 0, 0],
[0, 0, 0, 0, 0, Dt+K2/Te, -Kc/Te*delta, -Kc/Te*delta, -Kp/Te*delta];
module R = transpose(RR);
view(iostruct(R));
tst_status(1);$
