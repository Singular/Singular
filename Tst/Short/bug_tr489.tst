LIB "tst.lib";
tst_init();

option(noredefine);option(redSB);LIB "matrix.lib";

ring homalg_variable_140 = (2),(x1,x2,x3,x4,x5),dp;
setring homalg_variable_140;
short=0;option(redTail);
matrix homalg_variable_193[5][1] = x1^2+x2^2+x3^2+x4^2+x5^2+1,x1*x2+x2*x3+x3*x4+x1*x5+x4*x5,x1*x3+x1*x4+x2*x4+x2*x5+x3*x5,x1*x3+x1*x4+x2*x4+x2*x5+x3*x5,x1*x2+x2*x3+x3*x4+x1*x5+x4*x5;
homalg_variable_193 = transpose(homalg_variable_193);
matrix homalg_variable_194 = std(homalg_variable_193);
primdecSY(homalg_variable_194);

tst_status(1);$
