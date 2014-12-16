New git-supported Development Model {#development_model_page}
===================================


* development is to be done within personal (github) forks of the Sources repository
* the main Sources repository hosts only _official_ master and spielwiese branches
* there are a few people with push access to the Sources repository, who handle pull requests to master and spielwiese (Hans, Cristian, Oleksandr)
* special repository for CI with TeamCity [(internal)](http://snork.mathematik.uni-kl.de:31111/): [Singular/spielwiese-ci](https://github.com/Singular/spielwiese-ci), see Oleksandr's [talk](http://www.mathematik.uni-kl.de/~motsak/talks/Software_development_CAS_Singular.pdf). The git commands for TC testing may look as follows:
  * `git remote add TC git@github.com:Singular/spielwiese-ci.git` # initial setup
  * `git push TC +HEAD:remote-run/your_TC_user_name/feature_name` # testing with TC after a local commit (on Linuxes)
  * `git push TC +HEAD:remote-run**/no-ntl/**your_TC_user_name/feature_name` # testing with TC after a local commit **without NTL** (on Linuxes)
  * `git push TC +HEAD:remote-run**/no-ntl-but-flint/**your_TC_user_name/feature_name` # testing with TC after a local commit **without NTL** but **with FLINT** (on Linuxes)
  * `git push TC +HEAD:remote-run**/osx/**your_TC_user_name/feature_name` # testing with TC after a local commit **on Mac OS X**
  * `git push TC +HEAD:remote-run**/debug/**your_TC_user_name/feature_name` # testing with TC after a local commit **with debug** (on Linuxes)
  * `git push TC +HEAD:remote-run**/static/**your_TC_user_name/feature_name` # testing with TC after a local commit **static build** without dynamic modules (on Linuxes)
  * Note that for this you need to be registered on TC (register at [(internal)](http://snork.mathematik.uni-kl.de:31111/login.html) and ask Oleksandr to assign you a proper developer's role)
