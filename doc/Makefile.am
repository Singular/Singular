dist_man1_MANS = Singular.man ESingular.man TSingular.man

# Docbuild scripts
EXTRA_DIST =					\
	Makefile-docbuild.in version.texi.in	\
	help.cnf				\
	doc2idx.pl				\
	doc2tex.pl				\
	pl2doc.pl

# Text
EXTRA_DIST +=					\
	Building-Singular-from-source.md	\
	C-STYLEGUIDE.md				\
	C_CPP_StyleGuide.md			\
	Development-model.md			\
	How-To-Contribute.md			\
	Porting-instructions.md			\
	external-packages-dynamic-modules.md	\
	build_standards				\
	dir_structure				\
	DEPENDENCIES				\
	HOWTO-libsingular			\
	HOWTO.ispell				\
	STYLEGUIDE

# .doc files
EXTRA_DIST +=					\
	cones.no.doc				\
	cones.yes.doc				\
	countedref.doc				\
	decodegb.doc				\
	examples.doc				\
	general.doc				\
	letterplace.doc				\
	math.doc				\
	pdata.doc				\
	platform.doc				\
	plulibs.doc				\
	plural.doc				\
	pyobject.no.doc				\
	pyobject.yes.doc			\
	reference.doc				\
	sca.doc					\
	singular.doc				\
	start.doc				\
	ti_ip.doc				\
	tricks.doc				\
	types.doc


# Vendored copies
EXTRA_DIST +=					\
	texi2html				\
	texinfo.tex

# Other
EXTRA_DIST += 					\
	COPYING.texi				\
	Includes.dot				\
	Makefile-docbuild.in			\
	Makefile.lib2doc			\
	NEWS.texi				\
	bad_lib0.lib				\
	changes_in_singular4.texi		\
	elementsInTermsOfGeneratorsG25.sing	\
	images/Mybg.gif.uu			\
	images/a_begin.gif.uu			\
	images/a_begin_na.gif.uu		\
	images/a_document.gif.uu		\
	images/a_document_na.gif.uu		\
	images/a_empty.gif.uu			\
	images/a_end.gif.uu			\
	images/a_end_na.gif.uu			\
	images/a_help.gif.uu			\
	images/a_help_na.gif.uu			\
	images/a_index.gif.uu			\
	images/a_index_na.gif.uu		\
	images/a_left.gif.uu			\
	images/a_left_na.gif.uu			\
	images/a_leftdouble.gif.uu		\
	images/a_leftdouble_na.gif.uu		\
	images/a_page.gif.uu			\
	images/a_page_na.gif.uu			\
	images/a_right.gif.uu			\
	images/a_right_na.gif.uu		\
	images/a_rightdouble.gif.uu		\
	images/a_rightdouble_na.gif.uu		\
	images/a_search.gif.uu			\
	images/a_search_na.gif.uu		\
	images/a_searchdoc.gif.uu		\
	images/a_searchdoc_na.gif.uu		\
	images/a_tableofcon.gif.uu		\
	images/a_tableofcon_na.gif.uu		\
	images/a_top.gif.uu			\
	images/a_top_na.gif.uu			\
	images/a_up.gif.uu			\
	images/a_up_na.gif.uu			\
	images/bg.jpg.uu			\
	images/bg_left.gif.uu			\
	images/bg_right.gif.uu			\
	images/blue_dir.gif.uu			\
	images/blue_down.gif.uu			\
	images/blue_grab.gif.uu			\
	images/blue_help.gif.uu			\
	images/blue_next.gif.uu			\
	images/blue_nnext.gif.uu		\
	images/blue_pprev.gif.uu		\
	images/blue_prev.gif.uu			\
	images/blue_readme.gif.uu		\
	images/blue_top.gif.uu			\
	images/blue_up.gif.uu			\
	images/blue_uup.gif.uu			\
	images/contents_motif.gif.uu		\
	images/index_motif.gif.uu		\
	images/invisible.xbm.uu			\
	images/letterplace.jpg.uu		\
	images/next_motif.gif.uu		\
	images/next_motif_gr.gif.uu		\
	images/plural-2.jpg.uu			\
	images/plural.jpg.uu			\
	images/previous_motif.gif.uu		\
	images/previous_motif_gr.gif.uu		\
	images/sca.jpg.uu			\
	images/singular-1.jpg.uu		\
	images/singular-2.jpg.uu		\
	images/singular-3.jpg.uu		\
	images/singular-icon-transparent.gif.uu	\
	images/singular-small.jpg.uu		\
	images/singular.jpg.uu			\
	images/spacer3.gif.uu			\
	images/xy_contents.gif.uu		\
	images/xy_foot.gif.uu			\
	images/xy_next.gif.uu			\
	images/xy_next_gr.gif.uu		\
	images/xy_nextsection.gif.uu		\
	images/xy_nextsection_gr.gif.uu		\
	images/xy_previous.gif.uu		\
	images/xy_previous_gr.gif.uu		\
	images/xy_previoussection.gif.uu	\
	images/xy_previoussection_gr.gif.uu	\
	images/xy_up.gif.uu			\
	images/xy_up_gr.gif.uu			\
	latex2html.init				\
	lib2doc.texi				\
	manual-titlepage.tex			\
	memory/OMALLOC.texi			\
	orbits.gp				\
	orbitsG25.gp				\
	sample.lib				\
	sample_lib0.lib				\
	simple-make				\
	simplexOrbitRepresentativesG25.sing	\
	simplexSymmetryGroupG25.sing		\
	singcard.tex				\
	singular.dic				\
	singular.dot				\
	singular_toc.html			\
	t2h_singular.init			\
	t2h_standalone.init			\
	tutor-titlepage.tex			\
	tutor.tex				\
	usercard.tex				\
	version.texi.in

all-local:
	$(MAKE) -f Makefile-docbuild all

clean-local:
	$(MAKE) -f Makefile-docbuild clean

mostlyclean-local:
	$(MAKE) -f Makefile-docbuild mostlyclean

distclean-local:
	$(MAKE) -f Makefile-docbuild distclean

maintainer-clean-local:
	$(MAKE) -f Makefile-docbuild maintainer-clean

singular.hlp:
	$(MAKE) -f Makefile-docbuild $@
