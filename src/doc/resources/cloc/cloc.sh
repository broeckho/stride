#!/bin/bash
#############################################################################
# Copyright 2011-2016 Universiteit Antwerpen
#
# Licensed under the EUPL, Version 1.1 or  as soon they will be approved by 
# the European Commission - subsequent versions of the EUPL (the "Licence");
# You may not use this work except in compliance with the Licence.
# You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl5
#
# Unless required by applicable law or agreed to in writing, software 
# distributed under the Licence is distributed on an "AS IS" basis,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the Licence for the specific language governing
# permissions and limitations under the Licence.
#############################################################################


OUTPUTDIR=$1

if [ $# -eq 0 ]; then
	echo you need to specify an output directory as argument
	exit -1
fi
	
if [ ! -d $OUTPUTDIR ]; then
	echo cannot find output directory: $OUTPUTDIR
	echo cloc will create it if permission are ok
fi

cloc --out $OUTPUTDIR/cloc_summary_main.txt --exclude-dir src/main/resources/lib src/main

cloc --out $OUTPUTDIR/cloc_summary_test.txt --exclude-dir src/test/resources/lib src/test

cloc --out $OUTPUTDIR/cloc_by_file.xml --by-file --xml \
	--exclude-dir src/main/resources/lib,src/main/resources/data,src/doc,src/test/resources/lib,src/test/cpp/gtester/data src

xsltproc --output $OUTPUTDIR/cloc_overview.sc src/doc/resources/cloc/sloccount.xsl $OUTPUTDIR/cloc_by_file.xml

