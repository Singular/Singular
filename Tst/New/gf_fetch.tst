LIB "tst.lib"; tst_init();

ring S = (4,a),x,dp;print(S);

module N = gen(1);

ring D = (4,a),x,dp;print(D);

fetch(S, N); // BUG!

tst_status(1);$
