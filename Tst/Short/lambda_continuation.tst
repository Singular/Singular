LIB "tst.lib";
tst_init();

// feReadLine removes a trailing backslash and joins the next physical line.
// Parameter-list lambdas must follow the same established input rule.
proc continued_before_arrow=(int x)\
->{return(x+1);};
ASSUME(0,continued_before_arrow(1)==2);
proc continued_inside_arrow=(int x)-\
>{return(x+2);};
ASSUME(0,continued_inside_arrow(1)==3);
proc continued_before_body=(int x)->\
{return(x+3);};
ASSUME(0,continued_before_body(1)==4);
kill continued_before_arrow,continued_inside_arrow,continued_before_body;

print("lambda continuation tests passed");
tst_status(1);$
