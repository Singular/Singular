test(0);
"========= Beipiel Alex1 =============";
"=== elem 43, dim 1, mult 899 ========";
//=== 15: 1 min
//=== 17:  1 min 12 sec
//=== -,11,12: 48 sec
//=== 3 15: 38 sec
//=== 3 17: 37 sec

ring a1=32003,(t, x, y, z),dp;
ideal i=9x8+y7t3z4+5x4y2t2+2xy2z3t2,
        9y8+7xy6t+2x5y4t2+2x2yz3t2,
        9z8+3x2y3z2t4;
ideal i0=std(i);
;
";elem:",size(i0);
degree(i0);
i0;
kill i,i0;
kill a1;
LIB "tst.lib";tst_status(1);$
