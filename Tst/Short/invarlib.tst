LIB "tst.lib";
tst_init();

LIB "ainvar.lib";

example sortier; newline;
example derivate; newline;
example actionIsProper; newline;
example reduction; newline;
example completeReduction; newline;
example localInvar; newline;
example furtherInvar; newline;
example invariantRing; newline;

tst_status(1);
$

This is the file invarlib.tst
To run the tests and create the .res file 
go to the directory containing this file and type  
Singular -teqr12345678 invarlib.tst > invarlib.res
