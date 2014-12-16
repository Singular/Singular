LIB "tst.lib"; tst_init();
echo = 2;

LIB"singmathic.so";
LIB"general.lib";
option(redSB);
//listvar(Singmathic);
//option(prot);



proc mathicgb_prOrder0(def r)
{
}

proc mathicgb_setRingGlobal0()
{
}



proc makeComparable(ideal a) {return(sort(interred(a))[1]);}
proc same(ideal a, ideal b) {
  return(matrix(makeComparable(a)) == matrix(makeComparable(b)));
}
proc blah(def f) {
  f(a);
}

proc testGB(ideal i) {
  mathicgb_setRingGlobal();
  system("--ticks-per-sec", "100");
  system("--min-time", "0.0001");

  timer = 0;
  ideal A = mathicgb(i);
  printf(" MathicGB time in ms: %;", timer * 10);

  //A;  makeComparable(A);

  timer = 0;
  ideal B = std(i);
  printf(" Singular time in ms: %;", timer * 10);

  //B;  makeComparable(B);

  //makeComparable(A);"";makeComparable(B);

  printf(" Results equal: %;", same(A, B));
}

proc test1() {
  ring r = 32003,(x(1..5)),(a(2,3,4,5,6),rs);
  mathicgb_prOrder(r); testGB(cyclic(5));
}

proc test2() {
  ring r = 32003,(x(1..4)),lp;
  mathicgb_prOrder(r); testGB(cyclic(4));
}
proc test3() {
  ring r = 32003,(x(1..5)),Dp;
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test4() {
  ring r = 32003,(x(1..5)),Ds;
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test4x() {
  ring r = 32003,(x(1..5)),(a(2,3,4,5,6),Ds);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test5() {
  ring r = 32003,(x(1..5)),Wp(2,3,4,5,6);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test6() {
  ring r = 32003,(x(1..5)),Ws(2,3,4,5,6);
  mathicgb_prOrder(r); testGB(cyclic(5));
}

proc test7() {// doesn't match because interred doesn't work here
  ring r = 32003,(x(1..4)),Ws(-2,-3,-4,-5);
  mathicgb_prOrder(r); testGB(cyclic(4));
}

proc test8() {
  ring r = 32003,(x(1..5)),rp;
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test9() {
  ring r = 32003,(x(1..5)),rs;
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test10() {
  ring r = 32003,(x(1..5)),(a(2,3,4,5,6),rs);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test11() {
  ring r = 32003,(x(1..5)),dp;
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test12() {
  ring r = 32003,(x(1..5)),ds;
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test13() {
  ring r = 32003,(x(1..5)),(a(2,3,4,5,6),ds);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test14() {
  ring r = 32003,(x(1..5)),wp(2,3,4,5,6);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test15() {
  ring r = 32003,(x(1..5)),ws(2,3,4,5,6);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test16() {// doesn't match because interred doesn't work here
  ring r = 32003,(x(1..4)),ws(-2,-3,-4,-5);
  mathicgb_prOrder(r); testGB(cyclic(4));
}
proc test17() {
  ring r = 32003,(x(1..5)),ls;
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test18() {
  ring r = 32003,(x(1..5)),(a(2,3,4,5,6),ls);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test19() {
  ring r = 32003,(x(1..5)),(dp,c);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test20() {
  ring r = 32003,(x(1..5)),(dp,C);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test21() {
  ring r = 32003,(x(1..5)),(c,dp);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test22() {
  ring r = 32003,(x(1..5)),(C,dp);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test23() {
  ring r = 32003,(x(1..5)),(Dp,c);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test24() {
  ring r = 32003,(x(1..5)),(Dp,C);
  mathicgb_prOrder(r); testGB(cyclic(5));
}
proc test25() {
  ring r = 32003,(x(1..4)),(c,Dp);
  mathicgb_prOrder(r); testGB(cyclic(4));
}
proc test26() {
  ring r = 32003,(x(1..5)),(C,Dp);
  mathicgb_prOrder(r); testGB(cyclic(5));
}


//ring r = 32003,(a,b,c,d,e),(a(1,1,1,1,1),c,lp);
//mathicgb_setRingGlobal();
//mathicgb_prOrder(r); testGB(cyclic(2));
//quit;

test21();
test22();
test25();
test26();
//quit;

test1();
test2();
test3();
test4();
test4x();
test5();
test6();
test7(); // Singular wrongly thinks it is non-global
test8();
test9();
test10();
test11();
test12();
test13();
test14();
test15();
test16(); // Singular wrongly thinks it is non-global
test17();
test18();
test19();
test20();
//test21(); c-before
//test22(); c-before
test23();
test24();
//test25(); c-before
//test26(); c-before

//quit;

proc makeCyclic(int n) {
  ring r = 32003,(x(1..n)),dp;
  keepring r;
  mathicgb_prOrder(r);
  return(cyclic(n));
}

proc makeHCyclic(int n) {
  int k = n + 1;
  ring r = 32003,(x(1..k)),dp;
  keepring r;
  return(homog(cyclic(n), x(k)));
}

proc bench(int n) {
  printf("Testing cyclic-%;", n);
  testGB(makeCyclic(n));
  printf("Testing hcyclic-%;", n);
  testGB(makeHCyclic(n));
}

bench(7);


tst_status(1);$
quit;

int h = 1;
int n = 7;
int k=n+1;
ring r = 32003,(x(1..k)),dp;
homog(cyclic(n), x(k));
quit;

ring r = 32003, (a,b,c,d,e), dp;
ideal j = a2-b,a3-c;
ideal jGB = a2-b,ab-c,b2-ac;
ideal i = cyclic(5);
 a+b+c+d+e,
 ab+bc+cd+de+ea,
 abc+bcd+cde+dea+eab,
 abcd+bcde+cdea+deab+eabc,
 abcde-1;
i;
testGB(i);



exit;
"GB computed by MathicGB:";
ideal A=sort(interred(mathicgb(i)))[1];
"GB computed by std:";
ideal B=sort(interred(std(i)))[1];
matrix(A)==matrix(B);


exit;



// a2+ab+ac+b2+bc+c2, lex with a>b>c (from left)
ring rlp=101,(a,b,c),lp;" lp: lex from left";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rDp=101,(a,b,c),Dp;" Dp: 1-graded, lex from left";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rWp=101,(a,b,c),Wp(1,1,1);" Wp(v): v-graded, lex from left";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rDs=101,(a,b,c),Ds;" Ds: -1-graded, lex from left";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rWs=101,(a,b,c),Ws(1,1,1);" Ws(v): -v-graded, lex from left";a2+b2+c2+ab+ac+bc;1+b2+a+a2;

// a2+ab+b2+ac+bc+c2, revlex with a>b>c (from right)
ring rwp=101,(a,b,c),wp(1,1,1);" wp(v): v-graded, revlex from right";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rws=101,(a,b,c),ws(1,1,1);" ws(v): -v-graded, revlex from right";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rdp=101,(a,b,c),dp;" dp: 1-graded, revlex from right";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rrs=101,(a,b,c),rs;" rs: -1-graded, revlex from right";a2+b2+c2+ab+ac+bc;1+b2+a+a2;
ring rds=101,(a,b,c),ds;" ds: -1-graded, revlex from right";a2+b2+c2+ab+ac+bc;1+b2+a+a2;

// c2+bc+b2+ac+ab+a2, revlex with a<b<c (from left)
ring rls=101,(a,b,c),ls;" ls: revlex from left";a2+b2+c2+ab+ac+bc;1+b2+a+a2;

// c2+bc+ac+b2+ab+a2, lex with a<b<c (from right)
ring rrp=101,(a,b,c),rp;" rp: lex from right";a2+b2+c2+ab+ac+bc;1+b2+a+a2;


