#!/bin/bash

exit_on_error() {
   	retVal=$1
	if [ $retVal -ne 0 ]; then
    		echo "$2"
    		exit $retVal
	fi
}

REPO_PATH="/input/repo.tgz"
TMP_PATH="/tmp/tmpextract"
SRC_PATH="/src/"
TEST_EXECUTABLE_PATH="/src/test_mystring"
TEST_BINDING_PATH="/src/mystring"

TIMEOUT=10

# unpack repo archive
if [ ! -f "$REPO_PATH" ]; then
	exit_on_error 1 "[0!] Missing repo archive"
fi

mkdir $TMP_PATH
tar -xzf "$REPO_PATH" -C "$TMP_PATH"
cp -r ${TMP_PATH}/**/* "$SRC_PATH"

# build
MYSTRING_H_FILE_PATH="${SRC_PATH}mystring_h_path.txt"

if [ ! -f "$MYSTRING_H_FILE_PATH" ]; then
	exit_on_error 1 "[1!] Missing mystring_h_path.txt in repo root"
fi

MYSTRING_H=`cat $MYSTRING_H_FILE_PATH`

timeout $TIMEOUT clang++ -DIMYSTRING="\"${SRC_PATH}${MYSTRING_H}\"" -o "$TEST_EXECUTABLE_PATH" main.cpp tests/test_logger.cpp /src/MyString.cpp

if [ ! -f "$TEST_EXECUTABLE_PATH" ]; then
	exit_on_error 1 "[1!] Build failed"
fi
echo "[1+] Test harness was built successfully"

# run tests
TEST_RESULTS=`timeout $TIMEOUT $TEST_EXECUTABLE_PATH`
exit_on_error $? "Test program crashed"

TEST_ERRORS=`echo -e $TEST_RESULTS | grep -o "Error" | wc -l`
TEST_PASSED=`echo -e $TEST_RESULTS | grep -o "Success" | wc -l`

if [ $TEST_ERRORS -ne "0" ]; then
		echo "----- full test output -----"
    echo -e "$TEST_RESULTS"
    echo "-----------------------------"
    exit_on_error 1 "[2!] $TEST_ERRORS tests failed"
fi

echo "[2+] MyString tests passed"

# check memory leaks
timeout $TIMEOUT valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --xml=yes --xml-file=/tmp/valgrind-results.xml "$TEST_EXECUTABLE_PATH" > /dev/null

if [ ! -f /tmp/valgrind-results.xml ]; then
	exit_on_error 1 "[3!] Valgrind run failed"
fi

VALGRIND_RESULTS=`cat /tmp/valgrind-results.xml`
VALGRIND_ERRORS=`echo -e $VALGRIND_RESULTS | grep '<error>' | wc -l`

if [ $VALGRIND_ERRORS -ne "0" ]; then
	exit_on_error 1 "[3!] $VALGRIND_ERRORS valgrind errors detected (most likely memory leaks)"
fi

echo "[3+] Valgrind tests passed"

# test python bindings (pybind11)

## build binding
BINDING_PATH="${TEST_BINDING_PATH}$(/venv/bin/python3 -m pybind11 --extension-suffix)"
timeout $TIMEOUT clang++ -shared -DIMYSTRING="\"${SRC_PATH}${MYSTRING_H}\"" -O3 -std=c++17 -fPIC $(/venv/bin/python3 -m pybind11 --includes) -lpython3.12 /src/MyString.cpp /src/MyString_wrapper.cpp -o "$BINDING_PATH"

if [ ! -f "$BINDING_PATH" ]; then
        exit_on_error 1 "[1!] Binding build failed"
fi
echo "[4+] Python wrapper was built successfully"

cp -r /test/wrapper_test /src/wrapper_test
PYTEST_RESULTS=`cd /src/; timeout $TIMEOUT /venv/bin/pytest --rootdir /src/wrapper_test/ -v`
PYTEST_FAILURES=`echo -e $PYTEST_RESULTS | grep -o "FAILURE" | wc -l`

if [ $PYTEST_FAILURES -ne "0" ]; then
		echo "----- full test output -----"
    echo -e "$PYTEST_RESULTS"
    echo "-----------------------------"
	exit_on_error 1 "[5!] $PYTEST_FAILURES pytest tests for wrapper failed"
fi

echo "[5+] pytest tests for wrapper passed"

# output result
echo "[+] All tests passed"

exit 0

