noinst_LIBRARIES = libffatal.a

AM_CFLAGS = @TFS4_CFLAGS@

libffatal_a_SOURCES = \
	ffat_al.c

debug:
	@$(MAKE) CFLAGS="$(TFS4_DEBUG_CFLAGS)"

mostlyclean-generic:
	-rm -f *~ \#* .*~ .\#*

maintainer-clean_generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in

