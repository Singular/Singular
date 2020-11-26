LIB "tst.lib";
tst_init();

LIB "primdec.lib";
system("random",1760203212);
ring rng = 2,(x(1),x(2)),(lp(1),lp(1),C);
ideal I = x(2)^2+x(2)+1,x(1)^2*x(2)+1,0;
list ML1 = minAssGTZ(I, "facstd");
list ML2 = minAssGTZ(I, "GTZ" );
ML1;
ML2;
reduce( ML1[1], std(ML2[1]) );
reduce( ML2[1], std(ML1[1]) );
list RML1 = radical( ML1[1]);
list RML2 = radical( ML2[1]);

reduce( RML1[1], std(RML2[1]) );
reduce( RML2[1], std(RML1[1]) );
kill rng;

system("random",1416823952);
ring rng = 2,(x,y,z),(lp,C);
ideal I = x^2*y+x*z^2+1,x^2*z+z^2,z^3+1;

list ML1 =   minAssGTZ(I, "GTZ" );
list ML2 =   minAssGTZ(I, "facstd");
ML1;
ML2;
radical(ML1[1]);
radical(ML2[1]);
kill rng;

system("random",2064787217);
ring rng = 2,(x,y,z),(lp,C);
ideal I = z^3+1,x^2*y+y*z^2,y+1;

list ML2 =   minAssGTZ(I);
list ML3 =   minAssGTZ(I, "GTZ" );
ML2;
ML3;

radical(ML2[1]);
radical(ML3[1]);

idealsEqual( ML2[1], radical(ML2[1]) );
idealsEqual( ML3[1], radical(ML3[1]) );

tst_status(1);$
