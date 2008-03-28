/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifndef __CUT_PUBLIC_H__
#define __CUT_PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#  define CUT_GNUC_PRINTF(format_index, arg_index)                      \
    __attribute__((__format__ (__printf__, format_index, arg_index)))
#  define CUT_GNUC_NORETURN                     \
    __attribute__((__noreturn__))
#else
#  define CUT_GNUC_PRINTF(format_index, arg_index)
#  define CUT_GNUC_NORETURN
#endif

typedef struct _CutTestContext     CutTestContext;

typedef enum {
    CUT_TEST_RESULT_SUCCESS,
    CUT_TEST_RESULT_NOTIFICATION,
    CUT_TEST_RESULT_PENDING,
    CUT_TEST_RESULT_FAILURE,
    CUT_TEST_RESULT_ERROR
} CutTestResultStatus;

void  cut_test_context_pass_assertion       (CutTestContext *context);
void  cut_test_context_register_result      (CutTestContext *context,
                                             CutTestResultStatus status,
                                             const char *function_name,
                                             const char *filename,
                                             unsigned int line,
                                             const char *message,
                                             ...) /* CUT_GNUC_PRINTF(7, 8) */;
void  cut_test_context_long_jump            (CutTestContext *context) CUT_GNUC_NORETURN;

const char *cut_test_context_take_string    (CutTestContext *context,
                                             char           *string);
const char *cut_test_context_take_printf    (CutTestContext *context,
                                             char           *format,
                                             ...) CUT_GNUC_PRINTF(2, 3);
const char *cut_test_context_inspect_string_array (CutTestContext *context,
                                                   const char    **strings);


int   cut_utils_compare_string_array        (const char **strings1,
                                             const char **strings2);

void *cut_test_context_get_user_data        (CutTestContext *context);
int   cut_test_context_trap_fork            (CutTestContext *context,
                                             unsigned int time_out);

#ifdef __cplusplus
}
#endif

#endif /* __CUT_PUBLIC_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
