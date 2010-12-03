LIB "tst.lib";
tst_init();

ring r = (integer, 4), (x,y), lp;
poly f1 = 2*x*y;
poly f2 = 3*y^3+3;
poly f3 = x^2-3*x;
ideal I = f1,f2,f3;
ideal J = std(I); // is correct
J;

ideal F = f1,f2,f3;
module SF = syz(F);
print(SF);

print(transpose(matrix(SF))*transpose(matrix(F)));

ring r = (integer, 4), (x,y), (c,lp); // elim.mod.comp ordering
poly f1 = 2*x*y;
poly f2 = 3*y^3+3;
poly f3 = x^2-3*x;
ideal I = f1,f2,f3;
ideal J = std(I); // correct
ideal F = f1,f2,f3;
module FF = f1*gen(1) + gen(2), f2*gen(1) + gen(3), f3*gen(1)+gen(4);
print(std(FF));

ideal G = f1,f2;
module GG = f1*gen(1) + gen(2), f2*gen(1) + gen(3);
print(std(GG));


tst_status(1);$
