/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-readable-differ.h"
#include "cut-string-diff-writer.h"
#include "cut-utils.h"

G_DEFINE_TYPE(CutReadableDiffer, cut_readable_differ, CUT_TYPE_DIFFER)

static void diff        (CutDiffer     *differ,
                         CutDiffWriter *writer);
static void diff_line   (CutDiffer     *differ,
                         CutDiffWriter *writer,
                         gchar         *from_line,
                         gchar         *to_line);
static guint get_initial_context_size
                        (CutDiffer     *differ);

static void
cut_readable_differ_class_init (CutReadableDifferClass *klass)
{
    CutDifferClass *differ_class;

    differ_class = CUT_DIFFER_CLASS(klass);
    differ_class->diff = diff;
    differ_class->get_initial_context_size = get_initial_context_size;

    klass->diff_line = diff_line;
}

static void
cut_readable_differ_init (CutReadableDiffer *differ)
{
}

CutDiffer *
cut_readable_differ_new (const gchar *from, const gchar *to)
{
    return g_object_new(CUT_TYPE_READABLE_DIFFER,
                        "from", from,
                        "to", to,
                        NULL);
}

static void
mark_equal (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, " ", " ", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_EQUAL_LINE);
}

static void
mark_inserted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "+", " ", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_INSERTED_LINE);
}

static void
mark_deleted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "-", " ", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_DELETED_LINE);
}

static void
mark_difference (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "?", " ", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_DIFFERENCE_LINE);
}

static void
format_diff_point (CutDiffWriter *writer,
                   gchar *from_line, gchar *to_line,
                   gchar *from_tags, gchar *to_tags)
{
    gchar *lines[] = {NULL, NULL};

    from_tags = g_strchomp(from_tags);
    to_tags = g_strchomp(to_tags);

    lines[0] = from_line;
    mark_deleted(writer, lines, 0, 1);

    if (from_tags[0]) {
        lines[0] = from_tags;
        mark_difference(writer, lines, 0, 1);
    }

    lines[0] = to_line;
    mark_inserted(writer, lines, 0, 1);

    if (to_tags[0]) {
        lines[0] = to_tags;
        mark_difference(writer, lines, 0, 1);
    }
}

static void
append_spaces (GString *buffer, const gchar *string, guint begin, guint end)
{
    const gchar *last;

    last = g_utf8_offset_to_pointer(string, end);
    for (string = g_utf8_offset_to_pointer(string, begin);
         string < last;
         string = g_utf8_next_char(string)) {
        if (g_unichar_iswide_cjk(g_utf8_get_char(string))) {
            g_string_append(buffer, "  ");
        } else if (string[0] == '\t') {
            g_string_append_c(buffer, '\t');
        } else {
            g_string_append_c(buffer, ' ');
        }
    }
}

static void
append_character_n_times (GString *buffer, gchar character, guint n)
{
    guint i;

    for (i = 0; i < n; i++)
        g_string_append_c(buffer, character);
}

static void
diff_line (CutDiffer *differ, CutDiffWriter *writer,
           gchar *from_line, gchar *to_line)
{
    GString *from_tags, *to_tags;
    CutSequenceMatcher *matcher;
    const GList *operations;

    from_tags = g_string_new("");
    to_tags = g_string_new("");
    matcher =
        cut_sequence_matcher_char_new_full(from_line, to_line,
                                           cut_differ_util_is_space_character,
                                           NULL);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;
        guint from_width, to_width;

        from_width = cut_differ_util_compute_width(from_line,
                                                   operation->from_begin,
                                                   operation->from_end);
        to_width = cut_differ_util_compute_width(to_line,
                                                 operation->to_begin,
                                                 operation->to_end);
        switch (operation->type) {
        case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            append_spaces(from_tags, from_line,
                          operation->from_begin, operation->from_end);
            append_spaces(to_tags, to_line,
                          operation->to_begin, operation->to_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            append_character_n_times(to_tags, '+', to_width);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            append_character_n_times(from_tags, '-', from_width);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            append_character_n_times(from_tags, '^', from_width);
            append_character_n_times(to_tags, '^', to_width);
            break;
        default:
            g_error("unknown operation type: %d", operation->type);
            break;
        }
    }

    format_diff_point(writer, from_line, to_line, from_tags->str, to_tags->str);

    g_string_free(from_tags, TRUE);
    g_string_free(to_tags, TRUE);
    g_object_unref(matcher);
}

static void diff_lines (CutDiffer *differ, CutDiffWriter *writer,
                        gchar **from, gint from_begin, gint from_end,
                        gchar **to, gint to_begin, gint to_end);

static void
mark_diff_lines (CutDiffer *differ, CutDiffWriter *writer,
                 gchar **from, gint from_begin, gint from_end,
                 gchar **to, gint to_begin, gint to_end)
{
    if (from_begin < from_end) {
        if (to_begin < to_end) {
            diff_lines(differ, writer,
                       from, from_begin, from_end, to, to_begin, to_end);
        } else {
            mark_deleted(writer, from, from_begin, from_end);
        }
    } else {
        mark_inserted(writer, to, to_begin, to_end);
    }
}

static void
diff_lines (CutDiffer *differ, CutDiffWriter *writer,
            gchar **from, gint from_begin, gint from_end,
            gchar **to, gint to_begin, gint to_end)
{
    CutReadableDifferClass *klass;
    gdouble best_ratio, cut_off_ratio;
    gint from_equal_index, to_equal_index;
    gint from_best_index, to_best_index;
    gint to_index, from_index;

    klass = CUT_READABLE_DIFFER_GET_CLASS(differ);

    best_ratio = cut_differ_get_best_ratio(differ);
    cut_off_ratio = cut_differ_get_cut_off_ratio(differ);

    from_equal_index = to_equal_index = -1;
    from_best_index = from_begin;
    to_best_index = to_begin;

    for (to_index = to_begin; to_index < to_end; to_index++) {
        for (from_index = from_begin; from_index < from_end; from_index++) {
            CutSequenceMatcher *matcher;
            const gchar *from_line, *to_line;
            gdouble ratio;

            from_line = from[from_index];
            to_line = to[to_index];
            if (strcmp(from_line, to_line) == 0) {
                if (from_equal_index < 0)
                    from_equal_index = from_index;
                if (to_equal_index < 0)
                    to_equal_index = to_index;
                continue;
            }

            matcher = cut_sequence_matcher_char_new_full(
                from_line, to_line,
                cut_differ_util_is_space_character, NULL);
            ratio = cut_sequence_matcher_get_ratio(matcher);
            if (ratio > best_ratio) {
                best_ratio = ratio;
                from_best_index = from_index;
                to_best_index = to_index;
            }
            g_object_unref(matcher);
        }
    }

    if (best_ratio < cut_off_ratio) {
        if (from_equal_index < 0) {
            if (to_end - to_begin < from_end - from_begin) {
                mark_inserted(writer, to, to_begin, to_end);
                mark_deleted(writer, from, from_begin, from_end);
            } else {
                mark_deleted(writer, from, from_begin, from_end);
                mark_inserted(writer, to, to_begin, to_end);
            }
            return;
        }
        from_best_index = from_equal_index;
        to_best_index = to_equal_index;
        best_ratio = 1.0;
    }

    mark_diff_lines(differ, writer,
                    from, from_begin, from_best_index,
                    to, to_begin, to_best_index);
    klass->diff_line(differ, writer, from[from_best_index], to[to_best_index]);
    mark_diff_lines(differ, writer,
                    from, from_best_index + 1, from_end,
                    to, to_best_index + 1, to_end);
}

static void
diff_operations (CutDiffer *differ, CutDiffWriter *writer,
                 const GList *operations, gchar **from, gchar **to)
{
    for (; operations; operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;

        switch (operation->type) {
        case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            mark_equal(writer, from,
                       operation->from_begin, operation->from_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            mark_inserted(writer, to,
                          operation->to_begin, operation->to_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            mark_deleted(writer, from,
                         operation->from_begin, operation->from_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            diff_lines(differ, writer,
                       from, operation->from_begin, operation->from_end,
                       to, operation->to_begin, operation->to_end);
            break;
        default:
            g_error("unknown operation type: %d", operation->type);
            break;
        }
    }
}

static void
diff (CutDiffer *differ, CutDiffWriter *writer)
{
    CutSequenceMatcher *matcher;
    gchar **from, **to;

    from = cut_differ_get_from(differ);
    to = cut_differ_get_to(differ);
    matcher = cut_differ_get_sequence_matcher(differ);
    if (cut_differ_get_context_size(differ) == 0) {
        const GList *operations;
        operations = cut_sequence_matcher_get_operations(matcher);
        diff_operations(differ, writer, operations, from, to);
    } else {
        const GList *groups;

        groups = cut_sequence_matcher_get_grouped_operations(matcher);
        if (cut_differ_util_is_same_content(groups))
            return;

        /* write_header(priv, writer); */
        for (; groups; groups = g_list_next(groups)) {
            const GList *operations = groups->data;

            cut_differ_write_summary(differ, writer, operations);
            diff_operations(differ, writer, operations, from, to);
        }
    }
    cut_diff_writer_finish(writer);
}

static guint
get_initial_context_size (CutDiffer *differ)
{
    return 0;
}

gchar *
cut_diff_readable (const gchar *from, const gchar *to)
{
    CutDiffWriter *writer;
    CutDiffer *differ;
    gchar *diff;

    differ = cut_readable_differ_new(from, to);
    writer = cut_string_diff_writer_new();
    cut_differ_diff(differ, writer);
    diff = g_strdup(cut_string_diff_writer_get_result(writer));
    g_object_unref(writer);
    g_object_unref(differ);

    return diff;
}

gchar *
cut_diff_readable_folded (const gchar *from, const gchar *to)
{
    gchar *folded_from, *folded_to;
    gchar *folded_diff;

    folded_from = cut_utils_fold(from);
    folded_to = cut_utils_fold(to);
    folded_diff = cut_diff_readable(folded_from, folded_to);
    g_free(folded_from);
    g_free(folded_to);

    return folded_diff;
}

gboolean
cut_diff_readable_is_interested (const gchar *diff)
{
    if (!diff)
        return FALSE;

    if (!g_regex_match_simple("^[-+]", diff, G_REGEX_MULTILINE, 0))
        return FALSE;

    if (g_regex_match_simple("^[ ?]", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    if (g_regex_match_simple("(?:.*\n){2,}", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    return FALSE;
}

gboolean
cut_diff_readable_need_fold (const gchar *diff)
{
    if (!diff)
        return FALSE;

    if (g_regex_match_simple("^[-+].{79}", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    return FALSE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
