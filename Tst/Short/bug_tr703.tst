LIB "tst.lib";
tst_init();

ring rng = 0,(x,y),(ws(5,-9),C);
ideal J = x*y^4+x*y^3+x^3,
-y^4+x*y^2;
ideal gJ =  std(J);
ideal ggJ =  std(gJ);
gJ;
reduce (J,gJ );
ggJ;
reduce (J,ggJ );

ring r = 0,(x,y),(C,ws(5,-9));
ideal J = x*y^4+x*y^3+x^3,
-y^4+x*y^2;
ideal gJ =  std(J);
ideal ggJ =  std(gJ);
gJ;
reduce (J,gJ );
ggJ;
reduce (J,ggJ );

tst_status(1);$
