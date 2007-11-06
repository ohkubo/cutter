#include "cutter.h"
#include "cut-context-private.h"

void test_equal_int(void);
void test_equal_string(void);
void test_equal_double(void);
void test_error(void);
void test_fail(void);
void test_pending(void);

static void
dummy_error_test_function (void)
{
    cut_error("This test should error");
}

static void
dummy_fail_test_function (void)
{
    cut_fail("This test should fail");
}

static void
dummy_pending_test_function (void)
{
    cut_pending("This test has been pending ever!");
}

void
test_equal_int (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(-1, -1);
    cut_assert_equal_int(0, 0);
}

void
test_equal_string (void)
{
    cut_assert_equal_string("", "");
    cut_assert_equal_string("a", "a");
}

void
test_equal_double (void)
{
    cut_assert_equal_double(1.0, 0.1, 1.0);
    cut_assert_equal_double(1.0, 0.01, 1.01);
}

void
test_error (void)
{
    CutTest *test_object;
    CutContext *original_context, *test_context;
    gboolean ret;
    const CutTestResult *result;

    test_object = cut_test_new("dummy-error-test", dummy_error_test_function);
    cut_assert(test_object);

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);
    cut_context_set_test(test_context, test_object);

    original_context = cut_context_get_current();
    cut_context_set_current(test_context);
    ret = cut_test_run(test_object);
    cut_context_set_current(original_context);

    cut_assert(!ret);

    result = cut_test_get_result(test_object);
    cut_assert(result);

    cut_assert_equal_int(CUT_TEST_RESULT_ERROR, result->status);

    g_object_unref(test_object);
    g_object_unref(test_context);
}

void
test_pending (void)
{
    CutTest *test_object;
    CutContext *original_context, *test_context;
    gboolean ret;
    const CutTestResult *result;

    test_object = cut_test_new("dummy-pending-test", dummy_pending_test_function);
    cut_assert(test_object);

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);
    cut_context_set_test(test_context, test_object);

    original_context = cut_context_get_current();
    cut_context_set_current(test_context);
    ret = cut_test_run(test_object);
    cut_context_set_current(original_context);

    cut_assert(!ret);

    result = cut_test_get_result(test_object);
    cut_assert(result);

    cut_assert_equal_int(CUT_TEST_RESULT_PENDING, result->status);

    g_object_unref(test_object);
    g_object_unref(test_context);
}

void
test_fail (void)
{
    CutTest *test_object;
    CutContext *original_context, *test_context;
    gboolean ret;

    test_object = cut_test_new("dummy-fail-test", dummy_fail_test_function);
    cut_assert(test_object);

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);
    cut_context_set_test(test_context, test_object);

    original_context = cut_context_get_current();
    cut_context_set_current(test_context);
    ret = cut_test_run(test_object);
    cut_context_set_current(original_context);

    cut_assert(!ret);
    g_object_unref(test_object);
    g_object_unref(test_context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

