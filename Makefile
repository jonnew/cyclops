manual: MANUAL.md .pandoc_latex_template.latex
	pandoc MANUAL.md -o MANUAL.pdf --template=./.pandoc_latex_template.latex

clean:
	rm MANUAL.pdf
