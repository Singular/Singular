//
// test script for memory command
//
pagelength = 10000;
"Objects of SINGULAR use ",memory(0) ," bytes,",newline;
"allocated from system:", memory(1) , "bytes";
LIB "tst.lib";tst_status(1);$;
