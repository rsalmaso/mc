/*
   lib/vfs - vfs_path_t charset recode functions

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

#define TEST_SUITE_NAME "/lib/vfs"

#include "tests/mctest.h"

#include "lib/charsets.h"

#include "lib/strutil.h"
#include "lib/vfs/xdirentry.h"
#include "lib/vfs/path.h"

#include "src/vfs/local/local.c"

/* --------------------------------------------------------------------------------------------- */

/* @Mock */
const char *
mc_config_get_home_dir (void)
{
    return "/mock/home";
}

/* --------------------------------------------------------------------------------------------- */

static void
test_init_vfs (const char *encoding)
{
    str_init_strings (encoding);

    vfs_init ();
    vfs_init_localfs ();
    vfs_setup_work_dir ();

    mc_global.sysconfig_dir = (char *) TEST_SHARE_DIR;
    load_codepages_list ();
}

/* --------------------------------------------------------------------------------------------- */

static void
test_deinit_vfs (void)
{
    free_codepages_list ();
    str_uninit_strings ();
    vfs_shut ();
}

/* --------------------------------------------------------------------------------------------- */

/* @DataSource("test_path_recode_ds") */
static const struct test_path_recode_ds
{
    const char *input_codepage;
    const char *input_path;
    const char *expected_element_path;
    const char *expected_recoded_path;
} test_path_recode_ds[] = {
    {
        // 0.
        "UTF-8",
        "/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
        "/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
        "/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
    },
    {
        // 1.
        "UTF-8",
        "/#enc:KOI8-R/тестовый/путь",
        "/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
        "/#enc:KOI8-R/тестовый/путь",
    },
    {
        // 2.
        "KOI8-R",
        "/тестовый/путь",
        "/тестовый/путь",
        "/тестовый/путь",
    },
    {
        // 3.
        "KOI8-R",
        "/#enc:UTF-8/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
        "/тестовый/путь",
        "/#enc:UTF-8/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
    },
    {
        // 4. Test encode info at start
        "UTF-8",
        "#enc:KOI8-R/bla-bla/some/path",
        "/bla-bla/some/path",
        "/#enc:KOI8-R/bla-bla/some/path",
    },
};

/* @Test(dataSource = "test_path_recode_ds") */
START_PARAMETRIZED_TEST (test_path_recode, test_path_recode_ds)
{
    // given
    vfs_path_t *vpath;
    const char *element_path;
    const char *vpath_str;

    test_init_vfs (data->input_codepage);

    // when
    vpath = vfs_path_from_str (data->input_path);
    element_path = vfs_path_get_last_path_str (vpath);

    // then
    vpath_str = vfs_path_as_str (vpath);
    mctest_assert_ptr_ne (vpath, NULL);
    mctest_assert_str_eq (element_path, data->expected_element_path);
    mctest_assert_str_eq (vpath_str, data->expected_recoded_path);

    vfs_path_free (vpath, TRUE);
    test_deinit_vfs ();
}
END_PARAMETRIZED_TEST

/* --------------------------------------------------------------------------------------------- */

static struct vfs_class vfs_test_ops1;

/* @DataSource("test_path_to_str_flags_ds") */
static const struct test_path_to_str_flags_ds
{
    const char *input_path;
    const vfs_path_flag_t input_from_str_flags;
    const vfs_path_flag_t input_to_str_flags;
    const char *expected_path;
} test_path_to_str_flags_ds[] = {
    {
        // 0.
        "test1://user:passwd@127.0.0.1",
        VPF_NO_CANON,
        VPF_STRIP_PASSWORD,
        "test1://user@127.0.0.1/",
    },
    {
        // 1.
        "/test1://user:passwd@host.name/#enc:KOI8-R/тестовый/путь",
        VPF_NONE,
        VPF_STRIP_PASSWORD,
        "/test1://user@host.name/#enc:KOI8-R/тестовый/путь",
    },
    {
        // 2.
        "/test1://user:passwd@host.name/#enc:KOI8-R/тестовый/путь",
        VPF_NONE,
        VPF_RECODE,
        "/test1://user:passwd@host.name/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
    },
    {
        // 3.
        "/test1://user:passwd@host.name/#enc:KOI8-R/тестовый/путь",
        VPF_NONE,
        VPF_RECODE | VPF_STRIP_PASSWORD,
        "/test1://user@host.name/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
    },
    {
        // 4.
        "/mock/home/test/dir",
        VPF_NONE,
        VPF_STRIP_HOME,
        "~/test/dir",
    },
    {
        // 5.
        "/mock/home/test1://user:passwd@host.name/#enc:KOI8-R/тестовый/путь",
        VPF_NONE,
        VPF_STRIP_HOME | VPF_STRIP_PASSWORD,
        "~/test1://user@host.name/#enc:KOI8-R/тестовый/путь",
    },
    {
        // 6.
        "/mock/home/test1://user:passwd@host.name/#enc:KOI8-R/тестовый/путь",
        VPF_NONE,
        VPF_STRIP_HOME | VPF_STRIP_PASSWORD | VPF_HIDE_CHARSET,
        "~/test1://user@host.name/тестовый/путь",
    },
    {
        // 7.
        "/mock/home/test1://user:passwd@host.name/#enc:KOI8-R/тестовый/путь",
        VPF_NONE,
        VPF_STRIP_HOME | VPF_RECODE,
        "~/test1://user:passwd@host.name/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
    },
    {
        // 8.
        "/mock/home/test1://user:passwd@host.name/#enc:KOI8-R/тестовый/путь",
        VPF_NONE,
        VPF_STRIP_HOME | VPF_RECODE | VPF_STRIP_PASSWORD,
        "~/test1://user@host.name/\xD4\xC5\xD3\xD4\xCF\xD7\xD9\xCA/\xD0\xD5\xD4\xD8",
    },
};

/* @Test(dataSource = "test_path_to_str_flags_ds") */
START_PARAMETRIZED_TEST (test_path_to_str_flags, test_path_to_str_flags_ds)
{
    // given
    vfs_path_t *vpath;
    char *str_path;

    test_init_vfs ("UTF-8");

    vfs_init_class (&vfs_test_ops1, "testfs1", VFSF_NOLINKS | VFSF_REMOTE, "test1");
    vfs_register_class (&vfs_test_ops1);

    // when

    vpath = vfs_path_from_str_flags (data->input_path, data->input_from_str_flags);
    str_path = vfs_path_to_str_flags (vpath, 0, data->input_to_str_flags);

    // then
    mctest_assert_str_eq (str_path, data->expected_path);

    g_free (str_path);
    vfs_path_free (vpath, TRUE);
    test_deinit_vfs ();
}
END_PARAMETRIZED_TEST

/* --------------------------------------------------------------------------------------------- */

int
main (void)
{
    TCase *tc_core;

    tc_core = tcase_create ("Core");

    // Add new tests here: ***************
    mctest_add_parameterized_test (tc_core, test_path_recode, test_path_recode_ds);
    mctest_add_parameterized_test (tc_core, test_path_to_str_flags, test_path_to_str_flags_ds);
    // ***********************************

    return mctest_run_all (tc_core);
}

/* --------------------------------------------------------------------------------------------- */
