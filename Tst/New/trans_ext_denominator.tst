LIB "tst.lib"; tst_init();

LIB "polylib.lib"; // MASTER's denominator

ring Q=(0),x,dp;

number z = number(0) / number(1); z; // z == 0!

denominator(z); // OK!!!


ring Qs=(0,s),x,dp;

basering;

number z = number(0) / number(1); z; // z == 0!

denominator(z); // BUG due to trans. field ext. (but only wth gcc not with clang!)

tst_status(1);$

Singular : signal 11 (v: 4000):
current line:>>denominator(z); // ...
Segment fault/Bus error occurred (r:1377913790)
please inform the authors
trying to restart...

