LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring r=32003,(x,y,u,v),dp;
ring s=0,(a,b,c),wp(1,2,3);
ring t=0,x(1..5),(c,ls);
def R=ringtensor(r,s,t);
type R;
setring s;
ideal i = a2+b3+c5;
def S=changevar("x,y,z");       //change vars of s
setring S;
qring qS =std(fetch(s,i));      //create qring of S mod i (mapped to S)
def T=changevar("d,e,f,g,h",t); //change vars of t
setring T;
qring qT=std(d2+e2-f3);         //create qring of T mod d2+e2-f3
def Q=ringtensor(s,qS,t,qT);
setring Q; type Q;
kill R,S,T,Q;
tst_status(1);$
