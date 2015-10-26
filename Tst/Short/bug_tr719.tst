LIB "tst.lib";
tst_init();

// reduce (integers, mixed ordering) does not terminate
ring r=integer,(t,x(1..5)),ws(1,-1,-1,-1,-1,-1);
poly h = 3*t*x(2)*x(4)-21*t*x(1)*x(5)-12*t^2*x(1)*x(2)-2*t^2*x(1)*x(3)-6*t^2*x(1)*x(4);
ideal G = 
x(2)+6*x(4)+7*x(5)+4*t*x(2),
x(2)+8*x(3)+7*x(5)+t*x(2),
t*x(1)+3*t*x(2)+t^2*x(1),
t*x(4)+21*t*x(5)+12*t^2*x(2)+2*t^2*x(3)+7*t^2*x(4),
2-t;
G = std(G);
reduce(h,G);

tst_status(1);$

