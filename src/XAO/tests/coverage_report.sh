#!/bin/bash

# This script can be used to generate the code coverage report.
# Important: the library needs to be compiled in debug mode and with coverage option
#            add to configure options:
#                CXXFLAGS="-fprofile-arcs -ftest-coverage"
#                CFLAGS="-fprofile-arcs -ftest-coverage"

# to run the script:
# > cd BUILD/GEOM/src/XAO
# > make
# > cd tests
# > make
# > cp ../.libs/*.gcno .
# > ./TestXAO
# > cp ../.libs/*.gcda .
# > ./coverage_report XAO

QUIET=--quiet
# report directory
REPORT_PATH=report
# browser to open the report
BROWSER=firefox
# name for the info file
TITLE=$1

# initialize
if [[ $TITLE == "--reset" ]]
then
    lcov --base-directory . --directory . --zerocounters -q
    echo "Reset counters"
    exit 0
fi


if [ -z $TITLE ]
then
    echo $TITLE "name is required"
    exit 1
fi

INFO_FILE=${TITLE}.info
if [ -f $INFO_FILE ]
then
    rm -f $INFO_FILE
fi

# collecte des données
lcov --directory . --capture --output-file $INFO_FILE

# suppression des symboles externes
lcov --remove $INFO_FILE "/usr*" -o $INFO_FILE $QUIET
# prerequisites
lcov --remove $INFO_FILE "/home2/*" -o $INFO_FILE $QUIET

# suppression des tests
lcov --remove $INFO_FILE "*/tests/*" -o $INFO_FILE $QUIET

# génération du rapport
genhtml --output-directory $REPORT_PATH --title $TITLE --num-spaces 4 $INFO_FILE

if [ ! -z $BROWSER ]
then
    $BROWSER $REPORT_PATH/index.html &
fi