HowTo CONTRIBUTE to Singular development {#howto_contribute_page}
========================================

If you'd like to hack on Singular, you can follow the [instructions provided by github](http://help.github.com/fork-a-repo/)
and start by forking the Sources repo on GitHub:

<http://github.com/Singular/Sources>

See [here](@ref development_model_page) for a brief description of the git-supported development model we use.

For help with this procedure and general questions about development please write to
[libsingular-devel@googlegroups.com](https://groups.google.com/forum/#!forum/libsingular-devel).
Feel free to ask for comments or a review of your changes on the mailing list as well.

Before sending a pull request to merge your changes, make sure that

 * any new functions are properly documented
 * new functions are tested <br>
    * consider special cases! Optimally all decision branches of new functions should be covered by tests
 * the Singular test suite passes without failure. [Instructions on running the test suite](Running-test) is available on this wiki.

@note Almost everything GitHub-related can be done from command-line with the
following tool <http://hub.github.com>.

