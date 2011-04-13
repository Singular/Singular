# $Id$
#################################################
SHELL		= /bin/sh

##
## Primary targets
##

all: setup1 setup2 setup3 setup4
	cd src;make ntl.a || exit 1;cd ..

setup1:
	(cd src;make setup1 || exit 1;cd ..; touch setup1)

setup2: setup1
	(cd src;make setup2 || exit 1;cd ..; touch setup2)

setup3: setup2
	(cd src;make setup3 || exit 1;cd ..; touch setup3)

setup4: setup3
	(cd src;make setup4 || exit 1;cd ..; touch setup4)

install:	all
	(cd src;make install || exit 1;cd ..)

install_all: install

clean:
	cd src;make clean;cd ..
	-rm setup1 setup2 setup3 setup4
depend:
	@echo "No make depend available!"
