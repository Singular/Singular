LIB "tst.lib";
tst_init();
echo = 1;

LIB("primdec.lib");

ring rng = 0,x,dp;

ideal Z = 0;
def result = radicalEHV( Z );
ASSUME( 0, size(result)==0 );
ASSUME( 0, result[1]==0    );


ideal I = x, x+1;

result = radicalEHV( I );
ASSUME( 0, idealsEqual(result,ideal(1) ) );


tst_status(1); $
