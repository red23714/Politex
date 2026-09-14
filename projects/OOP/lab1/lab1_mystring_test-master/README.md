# MyString autotest

Test MyString implementation.

## Steps

1. Build MyString with test harness (./tests/ + main.cpp)

2. Test MyString implementation correctness (all methods) via test harness

3. Test MyString test harness for memory related errors (e.g. memory leaks) using valgrind

4. Build MyString Python wrapper (MyString_wrapper.cpp should be provided). Only pybind11 is supported

5. Test MyString Python wrapper via pytest (./wrppaer_test/)

## Requirements

1. mystring_h_path.txt file should be present in repo root and should contain relative path to MyString.h in repository directory structure

2. MyString_wrapper.cpp should be present for wrapper tests

3. MyString test harness, MyString Python binding building is limited to 10 seconds

4. Test runs are limited to 10 seconds

## How to run tests localy

NOTE: Docker is required

1. git clone 'this repo'

2. cd 'repo_dir'

3. docker build -t "mystringtest:latest" .

4. put source code into an archive with stucrue "./firstleveldir/MYSTRING_REPO_ROOT" (e.g. mylab.tgz)

5. docker run -v "./mylab.tgz:/input/repo.tgz:ro" mystringtest