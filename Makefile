#
# pour gettext, c'est un peu du n'importe quoi mais ca marche 
# si qqun comprend comment faire un Makefile simple pour gettext ..
#
RM = rm -f
RMR = rm -rf
# PDF_READER = /usr/bin/xpdf
PDF_READER = /usr/X11R6/bin/gv
CONF_LIB=conf/phpxref_pplib_doc.cfg
DOC_OUTPUT=doc/phpxref
PHPXREF=scripts/phpxref-0.2/phpxref.pl

.PHONY: clean distclean doc-distclean locale-distclean all

all:
	@echo
	@echo "Possible target are :"
	@echo ". locale"
	@echo ". doc"
	@echo ". doc-pdf"
	@echo ". bench"
	@echo
clean:
	$(RM) *~


distclean: clean doc-distclean locale-distclean

#
# locale
#
locale: locale-symlinks xgettext msgmerge msgfmt

locale-distclean:
	$(RM) fr

locale-symlinks:
	test -L fr || ln -s locale/fr/LC_MESSAGES/pp_lib.po fr

locale-undo:
	$(RM) locale/fr/LC_MESSAGES/pp_lib.po
	cvs update locale/fr/LC_MESSAGES/pp_lib.po

xgettext:
	chmod +x scripts/xgettext.sh
	test -d locale/po || mkdir locale/po
	echo > locale/po/pp_lib.po
	scripts/xgettext.sh

msgmerge:
	chmod +x scripts/msgmerge.sh
	scripts/msgmerge.sh

msgfmt:
	chmod +x scripts/msgfmt.sh
	scripts/msgfmt.sh

#
# doc
#
doc: phpxref_pplib_doc
	cd doc/lib-doc && make

doc-pdf: 
	cd doc/lib-doc && make pdf
	$(PDF_READER) doc/lib-doc/proportail.pdf

doc-distclean:
	$(RMR) $(DOC_OUTPUT)
	cd doc/lib-doc && make distclean

doc-re: doc-distclean doc

phpxref_pplib_doc:
	chmod +x $(PHPXREF)
	$(PHPXREF) -c $(CONF_LIB) $<

#
# bench
#
bench:
	cd scripts ; ./bench.php | lynx -stdin -dump ; cd ..
