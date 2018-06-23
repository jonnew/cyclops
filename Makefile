manual: MANUAL.txt
	pandoc MANUAL.txt --latex-engine=xelatex -o MANUAL.pdf

clean:
	rm MANUAL.pdf
