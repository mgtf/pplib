RM = rm -f
RMR = rm -rf
DVIPS = dvips
TEX = latex
PS2PDF = ps2pdf
TEXPDF = pdflatex
TEXHTML = latex2html -split 4
TEXASCII = pdftotext
TEXFILES = generalites.tex classes.tex applications_local.tex parc_machines.tex outils_dev.tex
all: ps pdf html clean

ps: proportail.ps
pdf: proportail.pdf
dvi: proportail.dvi
txt: proportail.txt

.PHONY: ps all pdf dvi txt print html clean distclean

html:
	$(TEXHTML) proportail.tex

clean:
	$(RM) *.aux *.log *~ *.toc logo.pdf

distclean: clean
	$(RM) *.dvi *.ps *.pdf proportail.txt 
	$(RMR) proportail

# logo:
#         $(PS2PDF) -sPAPERSIZE=a5 logo.eps
#
# image: logo
#

print: dvi
	dvips proportail.dvi

re: distclean all

%.dvi: %.tex $(TEXFILES)
	$(TEX) $<
	$(TEX) $<

%.ps: %.dvi %.tex $(TEXFILES)
	$(DVIPS) $< -o $@

%.pdf: %.ps $(TEXFILES)
	$(PS2PDF) $<
	make clean

%.txt: %.pdf $(TEXFILES)
	$(TEXASCII) $<
