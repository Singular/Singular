# $Id: Makefile,v 1.4 2004-01-29 14:05:39 Singular Exp $
#################################################
SHELL		= /bin/sh

##
## Primary targets
##

all: 
	cd src;$(MAKE) ntl.a || exit 1;cd ..

install:	all
	cd src;$(MAKE) install || exit 1;cd ..

clean:
	cd src;$(MAKE) clean;cd ..
