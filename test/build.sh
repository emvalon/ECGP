#!/bin/bash
rm -rf bin
mkdir bin
cd bin
cmake ..
make

if [ $? != 0 ]; then
    exit 1
fi

./UnitTest

COVERAGE_FILE=lcov.info
REPORT_FOLDER=../coverage_report
lcov --rc lcov_branch_coverage=1 -c -d ./ -o ${COVERAGE_FILE}_tmp > /dev/null
lcov --rc lcov_branch_coverage=1  --remove ${COVERAGE_FILE}_tmp "*main.c" -o ${COVERAGE_FILE} > /dev/null
genhtml --rc genhtml_branch_coverage=1 ${COVERAGE_FILE} -o ${REPORT_FOLDER} > /dev/null
rm -rf ${COVERAGE_FILE}_tmp
# rm -rf ${COVERAGE_FILE}

if [[ "$OSTYPE" == "darwin"* ]]; then
    open ./${REPORT_FOLDER}/index.html
fi