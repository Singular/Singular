LIB "tst.lib";
tst_init();

ring rng = integer,(xe,xv),dp;

ideal j = 4*xv^3+3*xv^2, 3*xe*xv^2+xe-2*xv;
ideal i = -3, -3*xv-1, -3;

def q1 = quotient( std(j), std(i) );
def q2 = quotient( j, i );

def r1 = reduce( std(q1), std(q2) ); 
def r2 = reduce( std(q2), std(q1) ); 

ASSUME( 0, size(r1)==0 );
ASSUME( 0, size(r2)==0 );


tst_status(1);$

