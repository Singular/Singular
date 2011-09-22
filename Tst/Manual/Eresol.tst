LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^2-x(2)^3;
list L=Eresol(J);
"Please press return after each break point to see the next element of the output list";
L[1][1]; // information of the first chart, L[1] list of charts
~;
L[2]; // list of charts with information about sons
~;
L[3]; // invariant, "#" means solved chart
~;
L[4]; // number of charts, 7 in this example
~;
L[5]; // height corresponding to each chart
~;
L[6]; // number of sons
~;
L[7]; // auxiliary invariant
~;
L[8]; // H exceptional divisors and more information
~;
L[9]; // complete resolution function
"Second example, write L[i] to see the i-th component of the list";
ring r = 0,(x(1..3)),dp;
ideal J=x(1)^2*x(2),x(3)^3; // SOLVED!
list L=Eresol(J);
L[4];  // 16 charts
L[9]; // complete resolution function
~;
"Third example, write L[i] to see the i-th component of the list";
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^3-x(1)*x(2)^3;
list L=Eresol(J);
L[4];  // 8 charts, rational exponents
L[9]; // complete resolution function
~;
tst_status(1);$
