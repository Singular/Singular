LIB "tst.lib"; tst_init();
LIB "inout.lib";
// can only be shown interactively, try the following commands:
// pause("press <return> to continue");
// pause();
// In the following pocedure TTT, xxx is printed and the execution of
// TTT is stopped until the return-key is pressed, if printlevel>0.
// xxx may be any result of a previous computation or a comment, etc:
//
// proc TTT
// { int pp = printlevel-voice+2;  //pp=0 if printlevel=0 and if TTT is
//    ....                         //not called from another procedure
//    if( pp>0 )
//    {
//       print( xxx );
//       pause("press <return> to continue");
//    }
//     ....
// }
tst_status(1);$
