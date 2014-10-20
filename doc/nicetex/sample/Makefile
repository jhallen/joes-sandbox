# Set document name here

DOC = sample

# Where are the tools?

TOOLS = ${HOME}

# Generate postscript and pdf files

all :
	@echo
	@echo "This is the ${DOC} document."
	@echo "The text is in ${DOC}.txt (edit with your favorite text editor)"
	@echo "The figures are in *.fig (edit with 'xfig')"
	@echo
	@echo "type:"
	@echo "  make ps        To make postscript file only"
	@echo "  make pdf       To make postscript file and pdf file"
	@echo "  make view      To make .pdf file and view it"
	@echo "  make print     To make postscript file an print it"
	@echo "  make submit    To copy .pdf file to document library"
	@echo "  make edit      To edit the text"
	@echo "  make clean     To clean up directory"
	@echo "  make help      Print nicetex help"
	@echo

# Make ps file

ps : ../${DOC}_out_tmp/${DOC}.ps

# Make pdf file

pdf : ../${DOC}_out_tmp/${DOC}.pdf

# Copy pdf file to library

submit : ../${DOC}_out_tmp/${DOC}.pdf
	cp ../${DOC}_out_tmp/${DOC}.pdf /var/www/html/library

# Edit the text

edit :
	${EDITOR} ${DOC}.txt

# View .pdf file

view : ../${DOC}_out_tmp/${DOC}.pdf
	(if which acroread; then acroread ../${DOC}_out_tmp/${DOC}.pdf& \
	 elif which /usr/local/Acrobat5/bin/acroread; then /usr/local/Acrobat5/bin/acroread ../${DOC}_out_tmp/${DOC}.pdf& \
	 else /usr/local/Acrobat4/bin/acroread ../${DOC}_out_tmp/${DOC}.pdf& fi)

# Print postscript file

print : ../${DOC}_out_tmp/${DOC}.ps
	lpr ../${DOC}_out_tmp/${DOC}.ps

# Print help

help :
	more ${TOOLS}/etc/nicetex.doc

# Convert dvi to postscript

../${DOC}_out_tmp/${DOC}.ps : ../${DOC}_out_tmp/${DOC}.dvi
	(cd ../${DOC}_out_tmp && \
	 dvips -P cmz ${DOC} -o)

# Merge table of contents into postscript, convert postscript to pdf

../${DOC}_out_tmp/${DOC}.pdf : ../${DOC}_out_tmp/${DOC}.ps ../${DOC}_out_tmp/${DOC}.bdy
	(cd ../${DOC}_out_tmp && \
	 ${TOOLS}/bin/tocinsert ${DOC}.toc `cat ${DOC}.bdy` <${DOC}.ps >${DOC}.nps && \
	 ps2pdf12 ${DOC}.nps ${DOC}.pdf)

# other ways to convert ps to pdf...
#	 if which distill_noz; then distill_noz <${DOC}.nps >${DOC}.pdf; \
#         else if which pstill; then pstill -o ${DOC}.pdf ${DOC}.nps; \

# Convert figures to .eps, convert .txt to .tex, run tex.

../${DOC}_out_tmp/${DOC}.dvi : ${DOC}.txt *.fig
	(mkdir ../${DOC}_out_tmp; true && \
	 ${TOOLS}/bin/fig ../${DOC}_out_tmp && \
	 ${TOOLS}/bin/nlink < ${DOC}.txt >../${DOC}_out_tmp/${DOC}.tx1 && \
	 ${TOOLS}/bin/npass < ../${DOC}_out_tmp/${DOC}.tx1 >../${DOC}_out_tmp/${DOC}.tx2 && \
	 ${TOOLS}/bin/nicetex < ../${DOC}_out_tmp/${DOC}.tx2 >../${DOC}_out_tmp/${DOC}.tex && \
	 cd ../${DOC}_out_tmp && \
	 tex ${DOC}.tex && \
	 tex ${DOC}.tex && \
	 tex ${DOC}.tex)

# Cleanup

clean :
	rm -f *~
	rm -f *.bak
