/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2009  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CUT_TEST_CASE_H__
#define __CUT_TEST_CASE_H__

#include <glib-object.h>

#include <cutter/cut-test-container.h>
#include <cutter/cut-test-iterator.h>

G_BEGIN_DECLS

#define CUT_TYPE_TEST_CASE            (cut_test_case_get_type ())
#define CUT_TEST_CASE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_CASE, CutTestCase))
#define CUT_TEST_CASE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_CASE, CutTestCaseClass))
#define CUT_IS_TEST_CASE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_CASE))
#define CUT_IS_TEST_CASE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_CASE))
#define CUT_TEST_CASE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_CASE, CutTestCaseClass))

typedef void (*CutStartupFunction)    (void);
typedef void (*CutShutdownFunction)   (void);

typedef struct _CutTestCaseClass CutTestCaseClass;

struct _CutTestCase
{
    CutTestContainer object;
};

struct _CutTestCaseClass
{
    CutTestContainerClass parent_class;

    void (*ready)         (CutTestCase    *test_case,
                           guint           n_tests);
    void (*start_test)    (CutTestCase    *test_case,
                           CutTest        *test,
                           CutTestContext *test_context);
    void (*complete_test) (CutTestCase    *test_case,
                           CutTest        *test,
                           CutTestContext *test_context);
    void (*start_test_iterator)
                          (CutTestCase    *test_case,
                           CutTestIterator *test_iterator,
                           CutTestContext *test_context);
    void (*complete_test_iterator)
                          (CutTestCase    *test_case,
                           CutTestIterator *test_iterator,
                           CutTestContext *test_context,
                           gboolean        success);

    void (*failure_in)    (CutTestCase    *test_case,
                           CutTestContext *test_context,
                           CutTestResult  *result);
    void (*error_in)      (CutTestCase    *test_case,
                           CutTestContext *test_context,
                           CutTestResult  *result);
    void (*pending_in)    (CutTestCase    *test_case,
                           CutTestContext *test_context,
                           CutTestResult  *result);
    void (*notification_in)
                          (CutTestCase    *test_case,
                           CutTestContext *test_context,
                           CutTestResult  *result);
    void (*omission_in)   (CutTestCase    *test_case,
                           CutTestContext *test_context,
                           CutTestResult  *result);
    void (*crash_in)      (CutTestCase    *test_case,
                           CutTestContext *test_context,
                           CutTestResult  *result);
};

GType        cut_test_case_get_type       (void) G_GNUC_CONST;

CutTestCase *cut_test_case_new            (const gchar *name,
                                           CutSetupFunction setup,
                                           CutTeardownFunction teardown,
                                           CutStartupFunction startup,
                                           CutShutdownFunction shutdown);
CutTestCase *cut_test_case_new_empty      (void);
void         cut_test_case_add_test       (CutTestCase *test_case,
                                           CutTest *test);
gboolean     cut_test_case_run            (CutTestCase   *test_case,
                                           CutRunContext *run_context);
gboolean     cut_test_case_run_test       (CutTestCase   *test_case,
                                           CutRunContext *run_context,
                                           const gchar   *name);
gboolean     cut_test_case_run_with_filter(CutTestCase   *test_case,
                                           CutRunContext *run_context,
                                           const gchar  **test_names);

void         cut_test_case_run_setup      (CutTestCase    *test_case,
                                           CutTestContext *test_context);
void         cut_test_case_run_teardown   (CutTestCase    *test_case,
                                           CutTestContext *test_context);

G_END_DECLS

#endif /* __CUT_TEST_CASE_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
