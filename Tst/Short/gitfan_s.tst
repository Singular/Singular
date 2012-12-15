LIB "tst.lib";
tst_init();

LIB "gitfan.lib";


/***
 * trivial example
 **/

ring R = 0,T(1..4),dp;

ideal a =
T(1)*T(2)+T(3)*T(4) - 1;

bigintmat Q[1][4] =
1,-1,1,-1;

fan F = gitFan(a,Q); // hier sollte der Kegel \mathbb{Q} rauskommen.


/***
 * Pluecker-Ideal G(2,4) mit dreidimensionaler Wirkung
 **/

ring R = 0,T(1..6),dp;

ideal a =
T(1)*T(6)-T(2)*T(5)+T(3)*T(4);

bigintmat Q[3][6] =
1, 0, 0, 1, 1, 0,
0, 1, 0, 1, 0, 1,
0, 0, 1, 0, 1, 1;

fan F = gitFan(a, Q); // 4 max. Kegel erwartet

kill a; kill R; kill Q; kill F;


/***
 * Pluecker-Ideal G(2,4) mit voller vierdimensionaler Wirkung
 **/

ring R = 0,T(1..6),dp;

ideal a =
T(1)*T(6)-T(2)*T(5)+T(3)*T(4);

bigintmat Q[4][6] =
1, 1, 1, 0, 0, 0,
1, 0, 0, 1, 1, 0,
0, 1, 0, 1, 0, 1,
0, 0, 1, 0, 1, 1;

fan F = gitFan(a, Q); // 8 max. Kegel erwartet

kill a; kill R; kill Q; kill F;


/***
 * Pluecker-Ideal G(2,5) mit vierdimensionaler Wirkung
 **/

bigintmat Q[4][10] =
1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
0, 1, 0, 0, 1, 0, 0, 1, 1, 0,
0, 0, 1, 0, 0, 1, 0, 1, 0, 1,
0, 0, 0, 1, 0, 0, 1, 0, 1, 1;

ring R = 0,T(1..10),dp;

ideal a =
T(2)*T(10)-T(3)*T(9)+T(4)*T(8),
T(1)*T(10)-T(3)*T(7)+T(4)*T(6),
T(1)*T(9)-T(2)*T(7)+T(4)*T(5),
T(1)*T(8)-T(2)*T(6)+T(3)*T(5),
T(5)*T(10)-T(6)*T(9)+T(7)*T(8);

fan F = gitFan(a, Q); // 12 max. Kegel erwartet


tst_status(1);$
