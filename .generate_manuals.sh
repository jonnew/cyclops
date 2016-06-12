#!/bin/bash

# Editable manual is MANUAL.md

# Generate readable html manual
pandoc MANUAL.md -o MANUAL.html -f markdown_strict

# Generate readable PDF manual
pandoc MANUAL.md -o MANUAL.pdf --template=.pandoc_latex_template.latex 
