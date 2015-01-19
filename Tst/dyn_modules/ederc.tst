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

noop();

ring r = 0,(x, y, z, t), (dp, C);


DetailedPrint(r);

vector p = 3*(x +y +z) * gen(5);

DetailedPrint(p, 3);

// vector v =3 *xyz * [0, 0, 1, x, y, z];v;DetailedPrint(v, 2);

leadmonom(p);
leadcomp(p);
leadrawexp(p);


ideal i;

i[1]=xz2-y2t;
i[2]=x2y-z2t;
i[3]=yz3-x2t2;

DetailedPrint(i, 2);

// std(i);


def S = MakeInducedSchreyerOrdering(1); setring S;

DetailedPrint(basering);

module i;

i[1]=xz2-y2t;
i[2]=x2y-z2t;
i[3]=yz3-x2t2;

DetailedPrint(i, 2);


 // def l = res(i, 0); DetailedPrint(l);

def L =  lead(i);
L;

homog(L);
attrib(L, "isHomog");

DetailedPrint(L[1] + L[2] + L[3]);

SetInducedReferrence(L);

DetailedPrint(basering);


print( GetInducedData() );

"a??";
vector a = [-1,-2,-3,-4];
"a: ";
DetailedPrint(a, 4);

vector v =  yz3*gen(2)-x2t2*gen(2)+xz2*gen(4)-y2t*gen(4);
v;

DetailedPrint(v, 4);


DetailedPrint(yz3 * L[1]);

DetailedPrint(x2t2 * L[1]);

DetailedPrint(xz2 * L[3]);

DetailedPrint(y2t * L[3]);

DetailedPrint(yz3 * L[1] +  2* x2t2 * L[1] + 3 * xz2 * L[3] + 4*y2t * L[3], 4);



x2y*gen(2)+xz2*gen(3)-y2t*gen(3)-z2t*gen(2);
DetailedPrint(_);
// xz2*gen(3)+x2y*gen(2)-y2t*gen(3)-z2t*gen(2);


yz3*gen(2)-x2t2*gen(2)+xz2*gen(4)-y2t*gen(4);
DetailedPrint(_);
// xz2*gen(4)+yz3*gen(2)-y2t*gen(4)-x2t2*gen(2)


yz3*gen(3)-x2t2*gen(3)+x2y*gen(4)-z2t*gen(4);
DetailedPrint(_);
// x2y*gen(4)+yz3*gen(3)-z2t*gen(4)-x2t2*gen(3)

tst_status(1);$

exit;


