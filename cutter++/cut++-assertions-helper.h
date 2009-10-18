/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUTPP_ASSERTIONS_HELPER_H__
#define __CUTPP_ASSERTIONS_HELPER_H__

#include <string>
#include <sstream>
#include <cutter/cut-helper.h>

namespace cut
{
    template <class Type> void assert_equal(Type& expected, Type& actual,
                                            const char *expression_expected,
                                            const char *expression_actual)
    {
        if (expected == actual) {
            cut_test_pass();
        } else {
            std::ostringstream inspected_expected;
            std::ostringstream inspected_actual;
            std::ostringstream message;

            inspected_expected << expected;
            cut_set_expected(inspected_expected.str().c_str());

            inspected_actual << actual;
            cut_set_actual(inspected_actual.str().c_str());

            message << "<" << expression_expected << " == ";
            message << expression_actual << ">";
            cut_test_fail(message.str().c_str());
        }
    }
}

#endif /* __CUTPP_ASSERTIONS_HELPER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
