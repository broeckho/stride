#############################################################################
#  This file is part of the Stride software. 
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or any 
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#############################################################################

#-------------
# MACROS
#-------------

CLEANDIR = \
	echo cleaning up `pwd`; \
	rm -f core *~ *.bak *.backup *.I .*.d~; \
	rm -f *.log *.aux *.bbl *.blg *.dvi *.lof *.lot *.lol *.toc *.idx *.ilg *.ind; \
	rm -f *-eps-converted-to.pdf *.synctex.gz *.tps *.out

DISTCLEANDIR = \
	echo distcleaning `pwd`; $(CLEANDIR) && rm -f UserManual.pdf 

PDFLATEX = pdflatex -interaction=batchmode --enable-write18
BIBTEX   = bibtex -terse

#-------------
# Targets
#-------------
.PHONY: default all pdf clean distclean

default: all

clean:
	@ $(CLEANDIR)

distclean:
	@ $(DISTCLEANDIR)

all:  
	$(PDFLATEX) UserManual.tex
	$(BIBTEX) UserManual
	$(PDFLATEX) UserManual.tex
	$(PDFLATEX) UserManual.tex
	
#############################################################################
