LIB "tst.lib"; tst_init();

short;

proc Test()
{
  ":: TEST :: ";
  basering;
  "short value: ", short;

  "::::             pars: ";
  "(par(1)):                           ", (par(1));
  "(par(1)**2):                        ", (par(1)**2);
  "(par(1)**2 + 2*par(1)):             ", (par(1)**2 + 2*par(1));

  "::::             vars: ";
  "(var(1)):                           ", (var(1));
  "(var(1)**2):                        ", (var(1)**2);
  "(-2)*(var(1)**2):                   ", (-2)*(var(1)**2);

  "::::             both: ";
  "(par(1))*(var(1)):                  ", (par(1))*(var(1));
  "(par(1))*(var(1)**2):               ", (par(1))*(var(1)**2);
  "(par(1)**3)*(var(1)):               ", (par(1)**3)*(var(1));
  "(par(1)**3)*(var(1)**2):            ", (par(1)**3)*(var(1)**2);
  "(par(1)**2 + 2*par(1))*(var(1)**2): ", (par(1)**2 + 2*par(1))*(var(1)**2);
}

" ::::::::::::::::::::: SHORT!!! ::::::::::::::::::::: ";

ring r=(0,Q),(x),dp;
Test();
minpoly = par(1)^2 - par(1) + 1;
Test();
kill r;
ring r=(121,Q),(x),dp;
Test();
kill r;
ring r=(complex,30,Q),(x),dp;
Test();
kill r;


" ::::::::::::::::::::: PAR:LONG!! ::::::::::::::::::::: ";

ring r=(0,Q9),(x),dp;
Test();
minpoly = par(1)^2 - par(1) + 1;
Test();
kill r;
ring r=(121,Q9),(x),dp;
Test();
kill r;
ring r=(complex,30,Q9),(x),dp;
Test();
kill r;




" ::::::::::::::::::::: VAR:LONG!! ::::::::::::::::::::: ";

ring r=(0,Q),(x9),dp;
Test();
minpoly = par(1)^2 - par(1) + 1;
Test();
kill r;
ring r=(121,Q),(x9),dp;
Test();
kill r;
ring r=(complex,30,Q),(x9),dp;
Test();
kill r;




" ::::::::::::::::::::: LONG!! ::::::::::::::::::::: ";

ring r=(0,Q9),(x9),dp;
Test();
minpoly = par(1)^2 - par(1) + 1;
Test();
kill r;

ring r=(121,Q9),(x9),dp;
Test();
kill r;

ring r=(complex,30,Q9),(x9),dp;
Test();
kill r;



tst_status(1);$
