LIB "tst.lib";
tst_init();

LIB "realclassify.lib";

ring r = 0, (x,y,z), ds;
poly f = (x2+3y-2z)^2+xyz-(x-y3+x2z3)^3;
realclassify(f, "nice");

realmorsesplit(f);

ring s = 0, (x,y), ds;
poly f = x3+y4;
milnornumber(f);

poly g = x3+xy3;
determinacy(g);

tst_status(1);$
