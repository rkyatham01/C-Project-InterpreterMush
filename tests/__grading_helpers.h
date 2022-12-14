#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <criterion/criterion.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "mush.h"
#include "mush.tab.h"

#define MAX_BUFFER_LEN 200

extern STMT *mush_parsed_stmt;
extern void push_input(FILE *in);

int get_ms(struct timespec *);
PIPELINE* setup_pipeline(FILE *);
void ensure_jobs_init(void);
void ensure_jobs_fini(void);
void make_tests_dirs(void);
void redirect_stdin(void);
void assert_values_equal(long, long);
void assert_null(char *);
void assert_not_null(char *);
void assert_normal_exit(int);
void assert_success(int);
void assert_not_fail(int);
void assert_failure(int);
void assert_output_matches(int);
void assert_outfile_matches(char *, char *);
void assert_no_valgrind_errors(int status, char *);
void assert_string_matches(char *, char *);
void assert_timer_not_exceed_ms(int, int);
