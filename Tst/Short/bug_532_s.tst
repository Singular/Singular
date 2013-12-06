// when enabling warn option, segfault when calling 'genus(i,1)'
// option("warn"); 

LIB "tst.lib";
tst_init();


    LIB("normal.lib");

    ring r = 0,(z,x),dp;
    ideal i = z^5-z^3-z*x^5-x^9;
    genus(i); 
    dim( std(i) ); // =1  => call genus(i,1) is legitimate

    if (not ( genus(i) == genus(i, 1) ))  { ERROR(" bug in Normal::genus()"); }

tst_status(1); $


