LATEX = latexmk -pdf -silent

all: picoquant.pdf

%.pdf: %.tex
	${LATEX} $<

clean:
	rm -f *.aux *.blg *.log *.dvi *.pdf *.synctex.gz *.fls *.fdb_latexmk

