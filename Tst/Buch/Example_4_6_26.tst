LIB "tst.lib";
tst_init();

//=================================  example 4.6.26  =========================

ring   R=0,x(4..1),dp;
ideal  I=-x(1)*x(2)+x(1)*x(4)+x(3),
         -x(1)*x(2)-2*x(2)^2+x(3)*x(4)-1,
         -x(1)*x(2)*x(4)+x(1)*x(4)^2+x(1)*x(2)-x(2)*x(4)
         +x(4)^2+3*x(2);
matrix M=char_series(I);
ring   S=(0,x(4)),x(1..3),dp;//to see the result with re-
matrix M=imap(R,M);          //spect to the chosen ordering
M;

tst_status(1);$

