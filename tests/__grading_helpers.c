#include "__grading_helpers.h"

int get_ms(struct timespec * ts){
    int ms = (int) (ts->tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        ts->tv_sec++;
        ms = 0;
    }
    return ms;
}

PIPELINE *setup_pipeline(FILE *stream) {
    push_input(stream);
    yyparse();
    fclose(stream);
    assert_not_null((char *)mush_parsed_stmt);
    return mush_parsed_stmt->members.sys_stmt.pipeline;
}

void ensure_jobs_init(void) {
    jobs_init();
    make_tests_dirs();
}

void ensure_jobs_fini(void) {
    jobs_fini();
}

void make_tests_dirs(void) {
    int err = mkdir("tmp", 0777);
    if (err == -1 && errno != EEXIST) {
        perror("Could not make tmp directory.");
        cr_assert_fail("No tmp directory.");
    }
    err = mkdir("test_output", 0777);
    if (err == -1 && errno != EEXIST) {
        perror("Could not make test_output directory.");
        cr_assert_fail("No test_output directory.");
    }
}

void redirect_stdin(void) {
  int fd = open("/dev/null", O_RDONLY);
  if(fd == -1) {
    cr_assert_fail("Could not open /dev/null");
  }
  if(dup2(fd, 0) == -1) {
    cr_assert_fail("Could not redirect stdin");
  }
}

void assert_values_equal(long act, long exp) {
    cr_assert_eq(act, exp,
                 "Failed to get expected value. Got: %ld | Expected: %ld.",
                 act, exp);
}

void assert_null(char *buf) {
    cr_assert_null(buf, "String should be NULL if variable does not exist.");
}

void assert_not_null(char *buf) {
    cr_assert_not_null(buf, "Output to stream is NULL.");
}

void assert_normal_exit(int status)
{
    cr_assert(!WIFSIGNALED(status),
              "The program terminated with an unexpected signal (%d).\n",
              WTERMSIG(status));
    cr_assert_eq(status, 0,
                 "The program did not exit normally (status = 0x%x).\n",
                 status);
}

void assert_success(int code) {
    cr_assert_eq(code, EXIT_SUCCESS,
                 "Program exited with %d instead of EXIT_SUCCESS",
                 code);
}

void assert_not_fail(int code) {
    cr_assert_neq(code, -1,
                 "Program should not return -1.");
}

void assert_failure(int code) {
    cr_assert_eq(code, -1,
                 "Program exited with %d instead of -1",
                 code);
}

void assert_output_matches(int code) {
    cr_assert_eq(code, EXIT_SUCCESS,
                 "File output did not match reference output.\n");
}

void assert_outfile_matches(char *actual_file, char *expected_file)
{
    char cmd[500];
    sprintf(cmd,
            "diff --ignore-tab-expansion --ignore-trailing-space "
            "--ignore-space-change --ignore-blank-lines %s "
            "%s",
            actual_file, expected_file);
    int err = system(cmd);
    cr_assert_eq(err, 0,
                 "The output was not what was expected (diff exited with "
                 "status %d).\n",
                 WEXITSTATUS(err));
}

void assert_no_valgrind_errors(int status, char *err_file)
{
    cr_assert_neq(WEXITSTATUS(status), 37,
                  "Valgrind reported errors -- see %s",
                  err_file);
}

void assert_string_matches(char *act, char *exp) {
    cr_assert_str_eq(act, exp,
                     "Output string did not match expected string. Got: %s | Expected: %s",
                     act, exp);
}

void assert_timer_not_exceed_ms(int time, int limit) {
    cr_assert_leq(time, limit,
                  "Job took too long: %d ms | Limit: %d ms",
                  time, limit);
}
