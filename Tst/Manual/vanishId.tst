LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
ring r=3,(x(1..3)),dp;
//generate all 3-vectors over GF(3)
list points=pointsGen(3,1);
list points2=convPoints(points);
//grasps the first 11 points
list p=graspList(points2,1,11);
print(p);
//construct the vanishing ideal
ideal id=vanishId(p);
print(id);
tst_status(1);$
