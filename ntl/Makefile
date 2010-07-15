# $Id$
#################################################
SHELL		= /bin/sh

##
## Primary targets
##

all: setup1 setup2 setup3 setup4
	cd src;$(MAKE) ntl.a || exit 1;cd ..

setup1:
	(cd src;$(MAKE) setup1 || exit 1;cd ..; touch setup1)

setup2: setup1
	(cd src;$(MAKE) setup2 || exit 1;cd ..; touch setup2)

setup3: setup2
	(cd src;$(MAKE) setup3 || exit 1;cd ..; touch setup3)

setup4: setup3
	(cd src;$(MAKE) setup4 || exit 1;cd ..; touch setup4)

install:	all
	(cd src;$(MAKE) install || exit 1;cd ..)

install_all: install

clean:
	cd src;$(MAKE) clean;cd ..
	-rm setup1 setup2 setup3 setup4
depend:
	@echo "No make depend available!"