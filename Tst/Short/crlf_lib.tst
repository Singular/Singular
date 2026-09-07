LIB "tst.lib";
tst_init();

// On native Windows the text-mode writer creates a CRLF file.  The library
// scanner and the deferred procedure reader must agree on byte offsets.
string crlfLib = "crlf_library_test.lib";
write(":w " + crlfLib,
  "version=\"version crlf_library_test.lib 1.0.0.0 Sep_2026 \";",
  "category=\"Tests\";",
  "info=\"CRLF library loading regression test\";",
  "",
  "proc crlfLibraryTest(int value)",
  "{",
  "  int intermediate = value + 1;",
  "  return(intermediate);",
  "}");

LIB(crlfLib);
crlfLibraryTest(41);

int cleanup = system("sh", "rm -f " + crlfLib);
tst_status(1); $
