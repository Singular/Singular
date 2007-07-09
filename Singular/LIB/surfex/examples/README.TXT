surfex - interactive examples
-----------------------------

To try one example (e.g., cone.sux), just type in a shell:

surfex cone.sux


surfex - command line examples
------------------------------

There are also some examples on how to use the command line to produce some
images and movies (these require the external tool "convert" to be installed
on your system) using surfex.  
The command line examples are contained in the files:
commandline_examples
and 
make_cayley_...
There is no documentation of the command line way of using surfex yet; 
however, we hope that the examples will allow to use it nevertheless.


Just to give one example for the command line usage:

surfex -a -nw -e x^3+y^2-z^2 --outfile tst.jpg

Here, the command line arguments have the following meaning:
-a : use antialiasing to improve the quality of the result
-nw : do not open any windows
-e x^3+y^2-z^2 : use the equation x^3+y^2-z^2
-- outfile tst.jpg : write the image to the file tst.jpg
We did not specify any size, so the default size will be used.