#!/bin/sh

#  moc.sh
#  qsculpt
#
#  Created by Juan Cabral on 2/12/12.
#  Copyright (c) 2012 plastilinaware. All rights reserved.

# MOC=/opt/local/bin/moc
MOC=/usr/bin/moc

# SCRIPT_INPUT_FILE_COUNT
# SCRIPT_INPUT_FILE_0
# SCRIPT_OUTPUT_FILE_COUNT

echo Generating moc sources
for ((num=0; num<${SCRIPT_INPUT_FILE_COUNT};num++))
do
FILE=\$\{SCRIPT_INPUT_FILE_${num}\}
eval IFILE=${FILE}
FILE=\$\{SCRIPT_OUTPUT_FILE_${num}\}
eval OFILE=${FILE}
${MOC} -o${OFILE} ${IFILE}
done


# UIC=/opt/local/bin/uic
UIC=/usr/bin/uic

# SCRIPT_INPUT_FILE_COUNT
# SCRIPT_INPUT_FILE_0
# SCRIPT_OUTPUT_FILE_COUNT

echo Generating uic sources
for ((num=0; num<${SCRIPT_INPUT_FILE_COUNT};num++))
do
FILE=\$\{SCRIPT_INPUT_FILE_${num}\}
eval IFILE=${FILE}
FILE=\$\{SCRIPT_OUTPUT_FILE_${num}\}
eval OFILE=${FILE}
${UIC} -o ${OFILE} ${IFILE}
done