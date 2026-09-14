#ifndef TEST_H
#define TEST_H

#include "test_ctor.h"
#include "test_insert.h"
#include "test_getters.h"
#include "test_append.h"
#include "test_erase.h"
#include "test_replace.h"
#include "test_substring.h"

#include "test_op_assign.h"
#include "test_op_equality.h"
#include "test_op_index.h"
#include "test_op_plus.h"
#include "test_op_plusassing.h"

#include "test_capacity.h"

#include "test_find.h"

#include "test_except.h"

void test() {
    test_constructors();

    test_getters();

    test_insert_methods();

    test_append();
    test_erase();
    test_replace();
    test_substr();

    test_op_assign();
    test_op_equality();
    test_op_index();
    test_op_plus();
    test_op_plusassign();

    test_capacity();

    test_find();

    test_except();
}

#endif