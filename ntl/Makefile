# $Id: Makefile,v 1.5 2004-01-29 15:59:53 Singular Exp $
#################################################
SHELL		= /bin/sh

##
## Primary targets
##

all: setup1 setup1 setup3 setup4
	cd src;$(MAKE) ntl.a || exit 1;cd ..

setup1:
	cd src;$(MAKE) setup1 || exit 1;cd ..; touch setup1

setup2:
	cd src;$(MAKE) setup2 || exit 1;cd ..; touch setup2

setup3:
	cd src;$(MAKE) setup3 || exit 1;cd ..; touch setup3

setup4:
	cd src;$(MAKE) setup4 || exit 1;cd ..; touch setup4

install:	all
	cd src;$(MAKE) install || exit 1;cd ..

clean:
	cd src;$(MAKE) clean;cd ..
	-rm setup1 setup2 setup3 setup4
