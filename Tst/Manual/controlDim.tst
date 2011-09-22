LIB "tst.lib"; tst_init();
LIB "control.lib";
//a WindTunnel example
ring A = (0,a, omega, zeta, k),(D1, delta),dp;
module R;
R = [D1+a, -k*a*delta, 0, 0],
[0, D1, -1, 0],
[0, omega^2, D1+2*zeta*omega, -omega^2];
R=transpose(R);
view(R);
view(controlDim(R));
tst_status(1);$
