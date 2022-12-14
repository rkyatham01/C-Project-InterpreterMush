#include "__grading_helpers.h"

Test(blackbox_suite, source_file, .init=make_tests_dirs, .timeout=5)
{
    char *cmd = "ulimit -t 5; bin/mush < tests/rsrc/source_file.mush > \
                 test_output/source_file.out";
    char *cmp = "cmp test_output/source_file.out tests/rsrc/source_file.out";

    int return_code = WEXITSTATUS(system(cmd));
    assert_success(return_code);
    return_code = WEXITSTATUS(system(cmp));
    assert_success(return_code);
}

Test(blackbox_suite, valgrind_simple, .init=make_tests_dirs, .timeout=10)
{
    char *cmd = "ulimit -t 5; valgrind --leak-check=full --track-fds=yes \
                 --error-exitcode=37 \
                 bin/mush < tests/rsrc/valgrind_simple.mush > \
                 test_output/valgrind_simple.out 2> \
                 test_output/valgrind_simple.err";

    int return_code = WEXITSTATUS(system(cmd));
    assert_no_valgrind_errors(return_code, "test_output/valgrind_simple.err");
    assert_normal_exit(return_code);
    assert_outfile_matches("tests/rsrc/valgrind_simple.out",
                           "test_output/valgrind_simple.out");
}

Test(blackbox_suite, valgrind_complex, .init=make_tests_dirs, .timeout=10)
{
    char *cmd = "ulimit -t 5; valgrind --leak-check=full --track-fds=yes \
                 --error-exitcode=37 \
                 bin/mush < tests/rsrc/valgrind_complex.mush > \
                 test_output/valgrind_complex.out 2> \
                 test_output/valgrind_complex.err";

    int return_code = WEXITSTATUS(system(cmd));
    assert_no_valgrind_errors(return_code, "test_output/valgrind_complex.err");
    assert_normal_exit(return_code);
    assert_outfile_matches("tests/rsrc/valgrind_complex.out",
                           "test_output/valgrind_complex.out");
}
