#include "cutter.h"
#include "cut-stream-parser.h"

void test_xml (void);
void test_invalid_xml (void);

static CutStreamParser *parser;
static CutTestResult *result;

void
setup (void)
{
    parser = cut_stream_parser_new(NULL);
    result = NULL;
}

void
teardown (void)
{
    g_object_unref(parser);

    if (result)
        g_object_unref(result);
}

static void
collect_result (CutStreamParser *parser, CutTestResult *test_result,
                gpointer user_data)
{
    CutTestResult **data = (CutTestResult **)user_data;

    g_object_ref(test_result);
    *data = test_result;
}

void
test_xml (void)
{
    const gchar xml[] = "<result>\n"
                        "  <test-case>\n"
                        "    <name>dummy test case</name>\n"
                        "  </test-case>\n"
                        "  <test>\n"
                        "    <name>dummy-error-test</name>\n"
                        "    <description>Error Test</description>\n"
                        "    <option>\n"
                        "      <name>bug</name>\n"
                        "      <value>1234</value>\n"
                        "    </option>\n"
                        "  </test>\n"
                        "  <status>error</status>\n"
                        "  <detail>This test should error</detail>\n"
                        "  <backtrace>\n"
                        "    <file>test-cut-report-xml.c</file>\n"
                        "    <line>31</line>\n"
                        "    <info>dummy_error_test()</info>\n"
                        "  </backtrace>\n"
                        "  <elapsed>0.000100</elapsed>\n"
                        "</result>\n";
    CutTest *test;

    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_true(cut_stream_parser_parse(parser, xml, -1, NULL));
    cut_assert(result);

    test = cut_test_result_get_test(result);
    cut_assert(test);

    cut_assert_equal_string("dummy test case",
                            cut_test_result_get_test_case_name(result));
    cut_assert_equal_string("dummy-error-test",
                            cut_test_result_get_test_name(result));
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    cut_assert_equal_double(0.0001, 0.0, cut_test_result_get_elapsed(result));
    cut_assert_equal_int(31, cut_test_result_get_line(result));
    cut_assert_equal_string("test-cut-report-xml.c",
                            cut_test_result_get_filename(result));
    cut_assert_equal_string("dummy_error_test",
                            cut_test_result_get_function_name(result));
    cut_assert_equal_string("This test should error",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("1234",
                            cut_test_get_attribute(test, "bug"));
    cut_assert_equal_string("Error Test",
                            cut_test_get_description(test));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
