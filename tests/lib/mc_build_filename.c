/*
   lib - mc_build_filename() function testing

   Copyright (C) 2011-2025
   Free Software Foundation, Inc.

   Written by:
   Slava Zanko <slavazanko@gmail.com>, 2011, 2013

   This file is part of the Midnight Commander.

   The Midnight Commander is free software: you can redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   The Midnight Commander is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define TEST_SUITE_NAME "/lib/util"

#include "tests/mctest.h"

#include "lib/strutil.h"
#include "lib/util.h"

/* --------------------------------------------------------------------------------------------- */

static char *
run_mc_build_filename (int iteration)
{
    switch (iteration)
    {
    case 0:
        return mc_build_filename ("test", "path", (char *) NULL);
    case 1:
        return mc_build_filename ("/test", "path/", (char *) NULL);
    case 2:
        return mc_build_filename ("/test", "pa/th", (char *) NULL);
    case 3:
        return mc_build_filename ("/test", "#vfsprefix:", "path  ", (char *) NULL);
    case 4:
        return mc_build_filename ("/test", "vfsprefix://", "path  ", (char *) NULL);
    case 5:
        return mc_build_filename ("/test", "vfs/../prefix:///", "p\\///ath", (char *) NULL);
    case 6:
        return mc_build_filename ("/test", "path", "..", "/test", "path/", (char *) NULL);
    case 7:
        return mc_build_filename ("", "path", (char *) NULL);
    case 8:
        return mc_build_filename ("", "/path", (char *) NULL);
    case 9:
        return mc_build_filename ("path", "", (char *) NULL);
    case 10:
        return mc_build_filename ("/path", "", (char *) NULL);
    case 11:
        return mc_build_filename ("pa", "", "th", (char *) NULL);
    case 12:
        return mc_build_filename ("/pa", "", "/th", (char *) NULL);
    default:
        return NULL;
    }
}

/* @DataSource("test_mc_build_filename_ds") */
static const struct test_mc_build_filename_ds
{
    const char *expected_result;
} test_mc_build_filename_ds[] = {
    { "test/path" },
    { "/test/path" },
    { "/test/pa/th" },
    { "/test/#vfsprefix:/path  " },
    { "/test/vfsprefix://path  " },
    { "/test/prefix://p\\/ath" },
    { "/test/test/path" },
    { "path" },
    { "path" },
    { "path" },
    { "/path" },
    { "pa/th" },
    { "/pa/th" },
};

/* @Test(dataSource = "test_mc_build_filename_ds") */
START_PARAMETRIZED_TEST (test_mc_build_filename, test_mc_build_filename_ds)
{
    // given
    char *actual_result;

    // when
    actual_result = run_mc_build_filename (_i);

    // then
    mctest_assert_str_eq (actual_result, data->expected_result);

    g_free (actual_result);
}
END_PARAMETRIZED_TEST

/* --------------------------------------------------------------------------------------------- */

int
main (void)
{
    TCase *tc_core;

    tc_core = tcase_create ("Core");

    // Add new tests here: ***************
    mctest_add_parameterized_test (tc_core, test_mc_build_filename, test_mc_build_filename_ds);
    // ***********************************

    return mctest_run_all (tc_core);
}

/* --------------------------------------------------------------------------------------------- */
