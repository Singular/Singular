option(prot);
"========= Beipiel Alex2 ============";
"=== elem 97, dim 2, mult 13 ========";
//=== 15: 4 min 33 sec
//=== 17  4 min 54 sec
//=== -,11,12: 4 min 29  sec
//=== 3 15: 4 min 28 sec
//=== 3 17: 4 min 42 sec

ring a2=32003,(t, x, y, z),dp;
ideal i=2t3x5y2z+x2t5y+2x2y,
        2y3z2x+3z2t6x2y+9y2t3z,
        2t5z+y3x2t+z2t3y2x5;
ideal i0=std(i);
";elem:",size(i0);
degree(i0);
i0;
kill i,i0;
kill a2;

"========= Beipiel Alex3 ===========";
"=== elem 119, dim 2, mult 1 =======";
//=== 15: 1 min 31 sec
//=== 17  1 min 35 sec
//=== -,11,12: 1 min 32  sec
//=== 3 15: 1 min 22 sec
//=== 3 17: 1 min 23 sec

ring a3=32003,(t, x, y, z),dp;
ideal i=5t3x8y2z+x2t5y+2x2y+5t,
        7y6z2x+6x2y+9y2t3z+4t6x,
        4t6z+8y3x5t+2t3y2x3+3x;
ideal i0=std(i);
";elem:",size(i0);
degree(i0);
i0;
kill i,i0;
kill a3;

"========= Beipiel Alex4 =========";
"=== elem 80, dim 2, mult 67 =====";
//=== 15: 26 sec
//=== 17  27 sec
//=== -,11,12: 25  sec
//=== 3 15: 24 sec
//=== 3 17: 22  sec
//=== 13 19:   sec
ring a4=32003,(t, x, y, z),dp;
ideal i=4t4z3y+6z4x3t5+5x5z7,
        5t6z3y3x+5x2z4t3y7+4x5t5y,
        6z7t5y+6z3y2t4+2t8z6;
ideal i0=std(i);
";elem:",size(i0);
degree(i0);
i0;
kill i,i0;
kill a4;

"========= Beipiel Alex5 ========";
"=== elem 37, dim 1, mult 44 ====";
//=== 15: 39 sec
//=== 17   39 sec
//=== -,11,12: 37  sec
//=== 3 15: 42 sec
//=== 3 17: 33 sec
//=== 13 19:   sec
ring a5=32003,(t, x, y, z),dp;
ideal i=4t2z+6z3t+3z2,
        5t2z7y3x+5x2z4t3y+3t,
        6zt2y+2x+6z2y2t+2y;
ideal i0=std(i);
";elem:",size(i0);
degree(i0);
i0;
kill i,i0;
kill a5;

"========= Beipiel Alex6 =======";
"=== elem 28, dim 1, mult 55 ===";
//=== 15: 1 min 47 sec
//=== 17   2 min 14 sec
//=== -,11,12: 1 min 40  sec
//=== 3 15: 1 min 19 sec
//=== 3 17: 1 min 20 sec
//=== 13 19:   sec
ring a6=32003,(t, x, y, z),dp;
ideal i=4t2z+6z3t+3z2tx+3z,
        5t7yx+5x2z4t3y+3t,
        6zt2y+2x+6z2y2t+2y;
ideal i0=std(i);
";elem:",size(i0);
degree(i0);
i0;
kill i,i0;
kill a6;
LIB "tst.lib";tst_status(1);$
