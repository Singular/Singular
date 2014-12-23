Addition of a new sub-package within `/kernel/` {#adding_kernel_package}
===============================================

   1. create a new directory (e.g. `/kernel/somepackage`) in `/kernel/` for your sub-package (`somepackage`)
   2. place all the necessary **sources** and **headers** there (`/kernel/somepackage`)
   3. `git add` all new **sources** and **headers**
   4. copy `/kernel/sample/Makefile.am` and adapt it:
     * replace *sample* with your sub-package name (e.g. `_libsample_` with `_libsomepackage_`)
     * update `libsomepackage_la_SOURCES` list
     * update `libsomepackage_la_include_HEADERS` and `libsomepackage_la_includedir`
     * `git add` this new `Makefile.am`
   5. change `/configure.ac` and `/kernel/Makefile.am` similar to [SAMPLE](https://github.com/Singular/Sources/commit/3074ad70db777694b401b1cd3980f74301b0339d) (NOTE: use your sub-package name *somepackage* instead of *sample*)
   6. commit all the changes and test it on TC, e.g. as indicated by our [Development Model](\ref development_model_page)


