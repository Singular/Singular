LIB "tst.lib"; tst_init();

proc Test()
{
"*******************************";
"ring: "; basering;

bigint a = 99999999999999999999999999;


number n = a;
a, "--> number ", string(n);
poly p = a;
a, "--> poly ", string(p);
vector v = a;
a, "--> vector ", string(v);

ideal I = a;
a, "--> ideal ", string(I);

module M = a;
a, "--> module ", string(M);


// int i = a;a, "--> int ", string(i); // not supported!

"";

}

ring R = (0),t,dp;Test();kill R;

ring R = (2),t,dp;Test();kill R;
ring R = (4,q),t,dp;Test();kill R;


ring R = (0,q),t,dp;Test();kill R;


ring R = (0,q),t,dp;minpoly=q2+1;Test();kill R; //????





ring R = (real),t,dp;Test();kill R;
ring R = (real,30,30),t,dp;Test();kill R;
ring R = (complex),t,dp;Test();kill R;








// ring R = (integer),t,dp;Test();kill R; // BUG!!!
// ring R = (integer,4),t,dp;Test();kill R; // ditto


tst_status(1);$
