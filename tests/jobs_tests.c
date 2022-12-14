#include "__grading_helpers.h"

Test(jobs_suite, simple_run, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 echo line 10\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    char buffer2[MAX_BUFFER_LEN] = {0};
    FILE *f = freopen("test_output/simple_run.out", "w+", stdout);
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    rewind(f);
    fgets(buffer2, MAX_BUFFER_LEN, f);
    assert_string_matches(buffer2, "line 10\n");
}

Test(jobs_suite, run_background, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "echo background &\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    char buffer2[MAX_BUFFER_LEN] = {0};
    FILE *f = freopen("test_output/run_background.out", "w+", stdout);
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    rewind(f);
    fgets(buffer2, MAX_BUFFER_LEN, f);
    assert_string_matches(buffer2, "background\n");
}

Test(jobs_suite, run_pipeline, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "echo system fundamentals | cat | wc \"-c\"\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    char buffer2[MAX_BUFFER_LEN] = {0};
    FILE *f = freopen("test_output/run_pipeline.out", "w+", stdout);
    PIPELINE *pp = setup_pipeline(stream);
    redirect_stdin();
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    rewind(f);
    fgets(buffer2, MAX_BUFFER_LEN, f);
    assert_string_matches(buffer2, "20\n");
}

Test(jobs_suite, output_redirection, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "echo redirected output > \"test_output/output_redirection.out\"\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    char buffer2[MAX_BUFFER_LEN] = {0};
    FILE *f = fopen("test_output/output_redirection.out", "r");
    fgets(buffer2, MAX_BUFFER_LEN, f);
    assert_string_matches(buffer2, "redirected output\n");
}

Test(jobs_suite, input_redirection, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "cat < \"tests/rsrc/input_redirection.in\"\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    char buffer2[MAX_BUFFER_LEN] = {0};
    FILE *f = freopen("test_output/input_redirection.out", "w+", stdout);
    PIPELINE *pp = setup_pipeline(stream);
    redirect_stdin();
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    rewind(f);
    fgets(buffer2, MAX_BUFFER_LEN, f);
    assert_string_matches(buffer2, "redirected input\n");
}

Test(jobs_suite, simple_expunge, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 sleep 1\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_expunge(job);
    assert_failure(ret);
    ret = jobs_wait(job);
    assert_success(ret);
    ret = jobs_expunge(job);
    assert_success(ret);
}

Test(jobs_suite, simple_wait, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 sleep 3 \n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_poll(job); // job still running
    assert_failure(ret);
    ret = jobs_wait(job);
    assert_success(ret);
    ret = jobs_poll(job); // job has finished
    assert_success(ret);
}

Test(jobs_suite, wait_expunged_job, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 sleep 1 \n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_poll(job);
    assert_failure(ret);
    ret = jobs_wait(job);
    assert_success(ret);
    ret = jobs_expunge(job);
    assert_success(ret);
    ret = jobs_wait(job);
    assert_failure(ret);
}

Test(jobs_suite, wait_invalid_job, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    int ret = jobs_wait(8);
    assert_failure(ret);
}

Test(jobs_suite, simple_poll, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 sleep 3\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_poll(job);
    assert_failure(ret);
    ret = jobs_wait(job);
    assert_success(ret);
    ret = jobs_poll(job);
    assert_success(ret);
}

Test(jobs_suite, poll_invalid, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    int ret = jobs_poll(7);
    assert_failure(ret);
}

Test(jobs_suite, simple_cancel, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 sleep 3\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    sleep(1); // wait for the job to actually be scheduled
    int ret = jobs_cancel(job);
    assert_success(ret);
    ret = jobs_cancel(job); // cancelling twice should fail
    assert_failure(ret);
}

Test(jobs_suite, cancel_invalid, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    int ret = jobs_cancel(7);
    assert_failure(ret);
}

Test(jobs_suite, cancel_terminated_job, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 sleep 3\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    ret = jobs_cancel(job);
    assert_failure(ret);
}

Test(jobs_suite, simple_pause, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "10 sleep 1\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_pause(); // wake up when sleep ends
    assert_success(ret);
}

Test(jobs_suite, simple_get_output, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "echo hello >@\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    char *output = jobs_get_output(job);
    assert_string_matches(output, "hello\n");
}

Test(jobs_suite, capture_long_output, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    int rand_buf_size = 4192;
    char rand_buf[rand_buf_size];
    char cpy_buf[rand_buf_size + 2];
    for (int i = 0; i < rand_buf_size; i++)
        rand_buf[i] = 'a' + rand() % 26;
    sprintf(cpy_buf, "%s\n", rand_buf);

    char buffer[5000];
    sprintf(buffer, "echo %s >@\n", rand_buf);
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    char *output = jobs_get_output(job);
    assert_string_matches(output, cpy_buf);
}

Test(jobs_suite, get_output_invalid, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "echo hi\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_wait(job);
    assert_success(ret);
    char *output = jobs_get_output(job);
    assert_null(output);
}

Test(jobs_suite, simple_show, .init=ensure_jobs_init, .fini=ensure_jobs_fini, .timeout=5)
{
    char buffer[MAX_BUFFER_LEN] = {0};
    sprintf(buffer, "date\n");
    FILE *stream = fmemopen(&buffer, strlen(buffer), "r");
    FILE *f = freopen("test_output/simple_show.out", "w+", stdout);
    PIPELINE *pp = setup_pipeline(stream);
    int job = jobs_run(pp);
    assert_not_fail(job);
    int ret = jobs_show(f);
    assert_success(ret);

    char jobid[10], pgid[10], status[10], pipeline[10];
    rewind(f);
    fscanf(f, "%s\t%s\t%s\t%s", jobid, pgid, status, pipeline);
    assert_not_null(jobid);
    assert_not_null(pgid);
    assert_not_null(status);
    assert_string_matches(pipeline, "date");
}
