manual: MANUAL.md .pandoc_latex_template.latex
	pandoc MANUAL.md -o MANUAL.pdf --template=./.pandoc_latex_template.latex --latex-engine=xelatex --from markdown-markdown_in_html_blocks+raw_html

clean:
	rm MANUAL.pdf
