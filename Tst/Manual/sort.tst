LIB "tst.lib"; tst_init();
LIB "general.lib";
ring r0 = 0,(x,y,z,t),lp;
ideal i = x3,z3,xyz;
sort(i);            //sorts using lex ordering, smaller polys come first
sort(i,3..1);
sort(i,"ls")[1];     //sort w.r.t. negative lex ordering
intvec v =1,10..5,2..4;v;
sort(v)[1];          // sort v lexicographically
sort(v,"Dp",1)[1];   // sort v w.r.t (total sum, reverse lex)
// Note that in general: lead(sort(M)) != sort(lead(M)), e.g:
module M = [0, 1, 1, 0], [1, 0, 0, 1]; M;
sort(lead(M), "c, dp")[1];
lead(sort(M, "c, dp")[1]);
// In order to sort M wrt a NEW ordering by considering OLD leading
// terms use one of the following equivalent commands:
module( M[ sort(lead(M), "c,dp")[2] ] );
sort( M, sort(lead(M), "c,dp")[2] )[1];
// BUG: Please, don't use this sort for integer vectors or lists
// with them if there can be negative integers!
// TODO: for some HiWi
sort(3..-3)[1];
sort(list(-v, v))[1];
tst_status(1);$
