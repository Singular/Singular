LIB "tst.lib"; tst_init();
// report value of all options
system("--");
// set minimal display time to 0.02 seconds
system("--min-time", "0.02");
// set timer resolution to 0.01 seconds
system("--ticks-per-sec", 100);
// re-seed random number generator
system("--random", 12345678);
// allow netscape to access HTML pages from the net
system("--allow-net", 1);
// and set help browser to netscape
system("--browser", "netscape");
tst_status(1);$
