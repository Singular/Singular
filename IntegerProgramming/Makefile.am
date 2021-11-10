ACLOCAL_AMFLAGS = -I ../m4

AM_CPPFLAGS = $(GMP_CPPFLAGS)
AM_LDFLAGS = $(GMP_LIBS)

noinst_HEADERS = list.h binomial__term_ordering.h BigInt.h LLL.h matrix.h ideal.h binomial__term_ordering.h ideal.h \
		term_ordering.h globals.h IP_algorithms.h binomial.h testdata.h
# Ikernel_to_send.c  # never used?

bindir=$(libexecdir)/singular/MOD

bin_PROGRAMS = toric_ideal solve_IP change_cost

COMMON_SOURCES = term_ordering.cc binomial.cc \
        list.cc BigInt.cc LLL.cc matrix.cc ideal.cc Buchberger.cc \
	ideal_stuff.cc IP_algorithms.cc testdata.cc

toric_ideal_SOURCES = $(COMMON_SOURCES) toric_ideal.cc
solve_IP_SOURCES = $(COMMON_SOURCES) solve_IP.cc
change_cost_SOURCES = $(COMMON_SOURCES) change_cost.cc
# gen_test_SOURCES = $(COMMON_SOURCES) gen_test.cc

# for now we don't install any additional help files...
EXTRA_DIST = README change_cost.hlp solve_IP.hlp toric_ideal.hlp gen_test.cc


