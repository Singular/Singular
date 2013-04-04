if HAVE_DOXYGEN

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

# all-local: doxyfile.stamp

html: doxyfile.stamp

clean-local:
	rm -rf html
endif 

EXTRA_DIST = Doxyfile.in
