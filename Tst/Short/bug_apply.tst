LIB "tst.lib";
tst_init();

apply( list() , x->{x} );
apply( list(1,2,"bla") , x->{x} );

tst_status(1);$
