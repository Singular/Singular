// Test File for 'zeroset.lib"
// rinvar.tst
// 
// Implementation by : Thomas Bayer
// Current Adress:
// Institut fuer Informatik, Technische Universitaet Muenchen
// www:    http://wwwmayr.informatik.tu-muenchen.de/personen/bayert/
// email : bayert@in.tum.de
//
// Last change 10.12.2000 
///////////////////////////////////////////////////////////////////////////////

LIB "tst.lib";
LIB "rinvar.lib";        

tst_init();
tst_ignore("CVS ID $Id: rinvar.tst,v 1.2 2005-04-25 10:11:18 Singular Exp $"); 
 

// example 1

ring S = 0,(s(1..4), t(1..6)), dp;
ideal G = s(1) - 1, s(2) - 2*s(3) - 3*s(4); 
ideal action = s(1)*t(1) + s(2)*t(2), s(1)*t(2), s(1)*t(3) + s(3)*t(4), s(1)*t(4), s(1)*t(5) + s(4)*t(6), s(1)*t(6);

ideal inv = NullCone(G,action);
inv;
kill S;
//tst_status();
 

// example 2

ring S = 0,(s(1..10), t(1..3)), dp;

ideal G3 = s(7)+s(8)+s(9)-1,s(4)+s(5)+s(6)-1,s(3)+s(6)+s(9)-1,s(2)+s(5)+s(8)-1,s(1)-s(5)-s(6)-s(8)-s(9)+1,s(9)^2-s(9),s(8)*s(9),s(6)*s(9),s(8)^2-s(8),s(6)*s(8)+s(5)*s(9)-s(5)-s(6)-s(8)-s(9)+1,s(5)*s(8),s(6)^2-s(6),s(5)*s(6),s(5)^2-s(5);
ideal G = G3, s(10)^3 - 1;
ideal action = s(10)*(t(1)*s(1)+t(2)*s(2)+t(3)*s(3)),s(10)*(t(1)*s(4)+t(2)*s(5)+t(3)*s(6)),s(10)*(t(1)*s(7)+t(2)*s(8)+t(3)*s(9));

def T = InvariantRing(G,action);
setring T;
invars;
groupid;

kill S;
kill T;
//tst_status();


// example 3

ring S = 0,(s(1..4),x,y),dp;
ideal action = s(1)*x+s(2)*y,s(3)*x+s(4)*y;
ideal G =s(3)*s(4),s(2)*s(4),s(3)^2+s(4)^2-1,s(1)*s(3),s(2)^2+s(4)^2-1,s(1)*s(2),s(1)^2-s(4)^2,s(4)^3-s(4),s(1)*s(4)^2-s(1);

def T = InvariantRing(G,action);
setring T;
invars;
groupid;

kill S;
kill T;
//tst_status();

// example 4

ring S = 0,(s(1..3), t(1..4)), dp;
ideal G = s(1)^4-1, s(2)^4-1, s(3)^4-1;
ideal action = s(1)^2*s(2)^2*s(3)^2*t(1), s(1)*s(2)^2*s(3)^2*t(2), s(1)^2*s(2)*s(3)^2*t(3), s(1)^2*s(2)^2*s(3)*t(4);

def T = InvariantRing(G,action);
setring T;
invars;
groupid;

kill S;
kill T;
//tst_status();
         

// example 5

ring S = (0,a), (s(1..4),t(1..2)), dp; 

ideal action = s(1)*t(1)+s(2)*t(2),s(3)*t(1)+s(4)*t(2);
ideal G = s(4)^5-s(4),s(3)*s(4),4*s(3)^4+(-4*a)*s(3)^3*s(4)^2+(-2*a+6)*s(3)^3*s(4)+(a-1)*s(3)^2*s(4)^3+(4*a-2)*s(3)^2*s(4)^2+(-a-5)*s(3)^2*s(4)+(-a+5)*s(3)*s(4)^3+6*s(3)*s(4)^2+(3*a+3)*s(3)*s(4)+4*s(4)^4-4,4*s(2)+4*s(3)^3+(-4*a)*s(3)^2*s(4)^2+(-2*a+6)*s(3)^2*s(4)+(a-1)*s(3)*s(4)^3+(4*a-2)*s(3)*s(4)^2+(-a-5)*s(3)*s(4),s(1)-s(4)^3;

def T = InvariantRing(G,action);
setring T;
invars;
groupid;

kill S;
kill T;
//tst_status();

// example 6

ring S = 0, (s(1..16),y(1..4)), dp; 

ideal I1 = s(13)+s(14)+s(15)+s(16)-1,3*s(9)+3*s(10)+3*s(11)+3*s(12)+s(13)+s(14)+s(15)+s(16)-4,3*s(5)+3*s(6)+3*s(7)+3*s(8)+s(13)+s(14)+s(15)+s(16)-4,s(4)+s(8)+s(12)+s(16)-1,s(3)+s(7)+s(11)+s(15)-1,s(2)+s(6)+s(10)+s(14)-1,s(1)+s(5)+s(9)+s(13)-1,s(16)^2-s(16),s(15)*s(16),s(14)*s(16),s(12)*s(16),2*s(8)*s(16)+s(12)*s(16),s(15)^2-s(15),s(14)*s(15),s(11)*s(15),2*s(7)*s(15)+s(11)*s(15),s(14)^2-s(14),s(11)*s(14)+s(12)*s(14)+s(10)*s(15)+s(12)*s(15)+s(10)*s(16)+s(11)*s(16)-s(10)-s(11)-s(12)-s(14)-s(15)-s(16)+1,s(10)*s(14),s(7)*s(14)+s(8)*s(14)+s(6)*s(15)+s(8)*s(15)+s(6)*s(16)+s(7)*s(16)-s(6)-s(7)-s(8)-s(14)-s(15)-s(16)+1;
ideal I2 = 2*s(6)*s(14)+s(10)*s(14),s(12)^2-s(12),s(11)*s(12)+s(15)*s(16),s(10)*s(12)+s(14)*s(16),s(8)*s(12)+s(8)*s(16)+s(12)*s(16),s(11)^2-s(11),s(10)*s(11)+s(14)*s(15),s(8)*s(11)+s(7)*s(12)+s(8)*s(15)+s(12)*s(15)+s(7)*s(16)+s(11)*s(16)-s(7)-s(8)-s(11)-s(12)-s(15)-s(16)+1,s(7)*s(11)+s(7)*s(15)+s(11)*s(15),s(10)^2-s(10),s(8)*s(10)+s(6)*s(12)+s(8)*s(14)+s(12)*s(14)+s(6)*s(16)+s(10)*s(16)-s(6)-s(8)-s(10)-s(12)-s(14)-s(16)+1,s(7)*s(10)+s(6)*s(11)+s(7)*s(14)+s(11)*s(14)+s(6)*s(15)+s(10)*s(15)-s(6)-s(7)-s(10)-s(11)-s(14)-s(15)+1,s(6)*s(10)+s(6)*s(14)+s(10)*s(14),s(8)^2-s(8),2*s(7)*s(8)+s(8)*s(11)+s(7)*s(12)+2*s(11)*s(12)+s(8)*s(15)+s(12)*s(15)+s(7)*s(16)+s(11)*s(16)+2*s(15)*s(16)-s(7)-s(8)-s(11)-s(12)-s(15)-s(16)+1;
ideal I3 = 2*s(6)*s(8)+s(8)*s(10)+s(6)*s(12)+2*s(10)*s(12)+s(8)*s(14)+s(12)*s(14)+s(6)*s(16)+s(10)*s(16)+2*s(14)*s(16)-s(6)-s(8)-s(10)-s(12)-s(14)-s(16)+1,s(7)^2-s(7),2*s(6)*s(7)+s(7)*s(10)+s(6)*s(11)+2*s(10)*s(11)+s(7)*s(14)+s(11)*s(14)+s(6)*s(15)+s(10)*s(15)+2*s(14)*s(15)-s(6)-s(7)-s(10)-s(11)-s(14)-s(15)+1,s(6)^2-s(6),s(6)*s(12)*s(15)+s(6)*s(11)*s(16)-s(6)*s(11)-s(6)*s(12)+s(7)*s(14)+s(8)*s(14)+s(8)*s(15)+s(7)*s(16)-s(7)-s(8)-s(14)-s(15)-s(16)+1;
ideal G = I1,I2,I3;

ideal action = y(1)*s(1)+y(2)*s(2)+y(3)*s(3)+y(4)*s(4),y(1)*s(5)+y(2)*s(6)+y(3)*s(7)+y(4)*s(8), y(1)*s(9)+y(2)*s(10)+y(3)*s(11)+y(4)*s(12),y(1)*s(13)+y(2)*s(14)+y(3)*s(15)+y(4)*s(16);

def T = InvariantRing(G,action);
setring T;
invars;
groupid;

kill S;
kill T;



// end

tst_status(1);$ 
