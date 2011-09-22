LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^3-x(1)*x(2)^3;
list L=Eresol(J);         // 8 charts, rational exponents
list B=genoutput(L[1],L[8],L[4],L[6],2,2,0); // generates the output
presentTree(B);
list iden0=collectDiv(B);
ResTree(B,iden0[1]);        // generates the resolution tree
// Use presentTree(B); to see the final charts
// To see the tree type in another shell
//    dot -Tjpg ResTree.dot -o ResTree.jpg
//   /usr/bin/X11/xv ResTree.jpg
tst_status(1);$
