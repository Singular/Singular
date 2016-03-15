LIB "tst.lib"; tst_init();

// These are some tests for computing Krull dimensions of ideals over integers
// The first one is the original bug found by Viktor Levandovskyy and Eva Zerz

ring r = integer,x,dp;
ideal i = 4,2x;
i = std(i);
dim(i);
// bug!! correct result = 1
kill r;

ring r = integer,(x,y),dp;
ideal i = 6x,4y;
i = std(i);
dim(i);
kill r;

ring r = integer,(x,y),dp;
ideal i = 6x10+xy,4y2+2y;
i = std(i);
dim(i);
kill r;

ring r = integer,(x,y,z),dp;
ideal i = 8x,12y,18z;
i = std(i);
dim(i);
kill r;

ring r = integer,(x,y,z),dp;
ideal i = 8x2+y,12y3+12z, 18z4+9x2;
i = std(i);
dim(i);
kill r;

ring r = (integer,24),x,dp;
ideal i = 4,2x;
i = std(i);
dim(i);
kill r;

ring r = (integer,24),(x,y),dp;
ideal i = 6x,4y;
i = std(i);
dim(i);
kill r;

ring r = (integer,24),(x,y),dp;
ideal i = 6x10+xy,4y2+2y;
i = std(i);
dim(i);
kill r;

ring r = (integer,24),(x,y,z),dp;
ideal i = 8x,12y,18z;
i = std(i);
dim(i);
kill r;

ring r = (integer,24),(x,y,z),dp;
ideal i = 8x2+y,12y3+12z, 18z4+9x2;
i = std(i);
dim(i);
kill r;

ring r = (integer),(x),dp;
dim(ideal(x));
dim(ideal(2x));
dim(ideal(2x,0));
dim(ideal(0,2x));
dim(ideal(6x));
dim(ideal(0,x));
kill r;

ring r=(integer,2,4),(x,y),dp;
dim(ideal(x));
kill r;

ring r=(integer,2,4),x,dp;
dim(std(ideal(x)));
kill r;

ring r=(integer,10),(x,y),dp;
dim(std(ideal(x)));
kill r;

ring r=(integer,2,4),x,dp; 
dim(std(ideal(x)));
kill r;

ring r=(integer,10),(x,y),dp;
dim(std(ideal(x)));
kill r;

ring r=(integer,2,4),x,dp;
dim(ideal(x,x));
dim(ideal(x));
dim(std(ideal(x)));
kill r;

tst_status(1);$
