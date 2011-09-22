LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring = 7,(x,y),ds;
list Hne=develop(4x98+2x49y7+x11y14+2y14);
print(Hne[1]);
// therefore the HNE is:
// z(-1)= 3*z(0)^7 + z(0)^7*z(1),
// z(0) = z(1)*z(2),       (there is 1 zero in the 2nd row before x)
// z(1) = z(2)^3*z(3),     (there are 3 zeroes in the 3rd row)
// z(2) = z(3)*z(4),
// z(3) = -z(4)^2 + 0*z(4)^3 +...+ 0*z(4)^8 + ?*z(4)^9 + ...
// (the missing x in the last line indicates that it is not complete.)
Hne[2];
param(Hne);
// parametrization:   x(t)= -t^14+O(t^21),  y(t)= -3t^98+O(t^105)
// (the term -t^109 in y may have a wrong coefficient)
displayHNE(Hne);
tst_status(1);$
