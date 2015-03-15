#!/bin/sh

#  BuildDocs.sh
#  qsculpt
#
#  Created by Juan Roberto Cabral Flores on 8/27/11.
#  Copyright 2011 plastilinaware. All rights reserved.

echo "Running Doxygen..."
/Applications/Doxygen.app/Contents/Resources/doxygen

# run make to build the doc set
cd docs/html
/usr/bin/make

#  Construct a temporary applescript file to tell Xcode to load a docset.

rm -f $TEMP_DIR/loadDocSet.scpt

echo "tell application \"Xcode\"" >> $TEMP_DIR/loadDocSet.scpt
echo "load documentation set with path \"/Users/rcabral/Library/Developer/Shared/Documentation/DocSets/org.doxygen.Project.docset\"" >> $TEMP_DIR/loadDocSet.scpt
echo "end tell" >> $TEMP_DIR/loadDocSet.scpt

#  Run the load-docset applescript command.

osascript $TEMP_DIR/loadDocSet.scpt

exit 0
