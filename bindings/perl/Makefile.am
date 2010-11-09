if PERL_BINDINGS

PERLMAKEFILE  = build-Text-ParseDelimited/Makefile
PERLDEPEND    = Text-ParseDelimited/Makefile.PL Text-ParseDelimited/typemap Text-ParseDelimited/lib/Text/ParseDelimited.pm
PERLCONFIG    = PREFIX="$(prefix)"

if DEBUG
PERLCONFIG    += OPTIMIZE=-g
endif

$(PERLMAKEFILE): $(PERLDEPEND) Makefile.am
	-test ! -d build-Text-ParseDelimited && cp -pr Text-ParseDelimited build-Text-ParseDelimited && chmod -R u+w build-Text-ParseDelimited && touch build-Text-ParseDelimited/.distclean
	cd build-Text-ParseDelimited && @PERL@ Makefile.PL $(PERLCONFIG)

all-local: $(PERLMAKEFILE)
	cd build-Text-ParseDelimited && $(MAKE) $(AM_MAKEFLAGS)

clean-local: $(PERLMAKEFILE)
	cd build-Text-ParseDelimited && $(MAKE) $(AM_MAKEFLAGS) clean

install-exec-local:
	cd build-Text-ParseDelimited && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" install

uninstall-local:
	rm -f "$(DESTDIR)$(prefix)/lib/*/site_perl/*/*/Text/ParseDelimited.pm"
	rm -f "$(DESTDIR)$(prefix)/lib/*/site_perl/*/*/auto/Text/ParseDelimited/ParseDelimited.*"
	rm -f "$(DESTDIR)$(prefix)/lib/*/site_perl/*/*/auto/Text/ParseDelimited/autosplit.ix"
	rm -f "$(DESTDIR)$(prefix)/lib/*/site_perl/*/*/auto/Text/ParseDelimited/.packlist"
	rm -f "$(DESTDIR)$(prefix)/lib/*/*/*/perllocal.pod"
	rm -f "$(DESTDIR)$(prefix)/share/man/man3/Text::ParseDelimited.3pm"

distclean-local:
	test -f build-Text-ParseDelimited/.distclean && rm -rf build-Text-ParseDelimited

endif

EXTRA_DIST = \
        Text-ParseDelimited/ppport.h \
        Text-ParseDelimited/ParseDelimited.xs \
        Text-ParseDelimited/lib/Text/ParseDelimited.pm \
        Text-ParseDelimited/Makefile.PL \
        Text-ParseDelimited/README \
        Text-ParseDelimited/Changes \
        Text-ParseDelimited/MANIFEST \
        Text-ParseDelimited/typemap
