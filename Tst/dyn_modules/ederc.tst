LIB "tst.lib"; tst_init();
/*
	    input ideal:

	    i[1]=xz2-y2t
	    i[2]=x2y-z2t
	    i[3]=yz3-x2t2

	    current ring:

	    //   characteristic : 0
	    //   number of vars : 4
	    //        block   1 : ordering IS(0)
	    //        block   2 : ordering dp
	    //                  : names    x y z t
	    //        block   3 : ordering C
	    //        block   4 : ordering IS(1)


	    trivial syzygies:

	    1. SYZ  x2y*gen(1)+xz2*gen(2)-y2t*gen(2)-z2t*gen(1)
	    2. SYZ  yz3*gen(1)-x2t2*gen(1)+xz2*gen(3)-y2t*gen(3)
	    3. SYZ  yz3*gen(2)-x2t2*gen(2)+x2y*gen(3)-z2t*gen(3)
*/
echo = 2;

LIB("syzextra.so");

ring r = 0,(x, y, z, t), (dp, C);


vector p = 3*(x +y +z) * gen(5);

leadmonom(p);
leadcomp(p);

ideal i;

i[1]=xz2-y2t;
i[2]=x2y-z2t;
i[3]=yz3-x2t2;

// std(i);


def S = MakeInducedSchreyerOrdering(1); setring S;

module i;

i[1]=xz2-y2t;
i[2]=x2y-z2t;
i[3]=yz3-x2t2;

def L =  lead(i);
L;

homog(L);
attrib(L, "isHomog");

SetInducedReferrence(L);

print( GetInducedData() );

"a??";
vector a = [-1,-2,-3,-4];
"a: ";

vector v =  yz3*gen(2)-x2t2*gen(2)+xz2*gen(4)-y2t*gen(4);
v;



x2y*gen(2)+xz2*gen(3)-y2t*gen(3)-z2t*gen(2);
// xz2*gen(3)+x2y*gen(2)-y2t*gen(3)-z2t*gen(2);


yz3*gen(2)-x2t2*gen(2)+xz2*gen(4)-y2t*gen(4);
// xz2*gen(4)+yz3*gen(2)-y2t*gen(4)-x2t2*gen(2)


yz3*gen(3)-x2t2*gen(3)+x2y*gen(4)-z2t*gen(4);
// x2y*gen(4)+yz3*gen(3)-z2t*gen(4)-x2t2*gen(3)

tst_status(1);$

exit;


