manual: MANUAL.txt
	pandoc MANUAL.txt -f markdown --latex-engine=xelatex -o MANUAL.pdf

clean:
	rm MANUAL.pdf
