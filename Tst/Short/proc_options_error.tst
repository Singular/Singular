LIB "tst.lib";
tst_init();

ring R=0,x,dp;
option(noredSB); option(noredTail); option(noredThrough); option(nointStrategy);
intvec options_before=option(get);

// A failed procedure must restore global and ring-dependent options.
proc fail_with_changed_options()
{
  option(redSB); option(redTail); option(redThrough); option(intStrategy);
  ERROR("expected test error");
}
fail_with_changed_options();
option(get)==options_before;

ring S=0,y,dp;
setring R;
option(get)==options_before;

// Option changes made by a successful procedure remain visible.
option(redSB);
intvec options_with_redSB=option(get);
option(noredSB);
proc set_redSB() { option(redSB); }
set_redSB();
option(get)==options_with_redSB;

tst_status(1);$
