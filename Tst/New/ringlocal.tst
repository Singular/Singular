LIB "tst.lib";
tst_init();

ring R = (integer,10),(x,y),ds;

ideal i = 2;
std(i);

i = 3;
std(i);

i = 2x;
std(i);

i = 3x;
std(i);

i = x2+x3+x4;
std(i);

i = 2x2+2x3+2x4;
std(i);

i = 3x2y3+2x3y4+5x4y10;
std(i);

ring R = (integer,100),(x,y),ds;
poly f1 = 5y3+2x5;
poly f2 = 3x5y2+25x2y10;
poly f3 = 10x13+50y6;
ideal i = f1,f2,f3;
ideal s=std(i);
reduce(f1*f2+f3,s);
reduce(2*f1*f2*f3+25,s);
reduce(2*f1+3*f3+10*f2,s);
reduce(f1+4,s);

ring R = (integer,100),(x,y),ds;
ideal i = 4y+25x5y3+27y13,x+76y14,2+84y10+75x13+67x4y13;
std(i);

ring R = (integer,44),(x,y),ds;
ideal i = 2x,11y;
i=std(i);
poly f = 2x+22y;
lift(i,f);

ring R = (integer,44),(x,y),ls;
poly f1 = 4y15+7xy3+2x10y4;
poly f2 = xy+13x7y5+40x9;
poly f3 = 4x16;
ideal i = f1,f2,f3;
i=std(i);
poly f = f1+4*f2+13*f3;
lift(i,f);

//New tests

ring R = (integer,6),(x,y),ds;
ideal i = 2x2-xy, 2xy-y2;
std(i);

ideal i = 2x2-xy, 3xy-y2;
std(i);

ideal i = 2x, 3y, xy;
std(i);

ideal i = 2x, 3y;
std(i);

ring R = (integer,44),(x,y),ds;
ideal i = 2x, 4y, 11xy;
std(i);


tst_status(1);
$
