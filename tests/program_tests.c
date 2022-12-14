#include "__grading_helpers.h"

Test(program_suite, prog_insert_beginning, .timeout=2)
{
    STMT *stmt = malloc(sizeof(STMT));
    stmt->lineno = 1;
    stmt->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt);
    assert_success(ret);
    free_stmt(stmt);
}

Test(program_suite, prog_insert_replace, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 1;   //replace 1
    stmt2->class = RUN_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    free_stmt(stmt2);
}

Test(program_suite, prog_insert_middle, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 3;
    stmt2->class = RUN_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    STMT *stmt3 = malloc(sizeof(STMT));
    stmt3->lineno = 2;   //2 goes between 1 and 3
    stmt3->class = RUN_STMT_CLASS;
    ret = prog_insert(stmt3);
    assert_success(ret);

    free_stmt(stmt1);
    free_stmt(stmt2);
    free_stmt(stmt3);
}

Test(program_suite, prog_insert_end, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;    //insert 2 at the end
    stmt2->class = RUN_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    free_stmt(stmt1);
    free_stmt(stmt2);
}

Test(program_suite, prog_list, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    char buffer1[MAX_BUFFER_LEN] = {0};
    char buffer2[MAX_BUFFER_LEN] = {0};
    sprintf(buffer2, "%7d\trun\n-->\n%7d\tlist\n", 1, 2);

    prog_reset(); // reset PC
    prog_next(); // at stmt 2

    FILE *f = fmemopen(&buffer1, sizeof(buffer1), "w");
    ret = prog_list(f);
    assert_success(ret);
    fclose(f);
    assert_string_matches(buffer1, buffer2);

    free_stmt(stmt1);
    free_stmt(stmt2);
}

Test(program_suite, prog_delete_all, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    STMT *stmt3 = malloc(sizeof(STMT));
    stmt3->lineno = 3;
    stmt3->class = RUN_STMT_CLASS;
    ret = prog_insert(stmt3);
    assert_success(ret);

    ret = prog_delete(1,3);
    assert_success(ret);
}

Test(program_suite, prog_delete_after_pc, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    STMT *stmt3 = malloc(sizeof(STMT));
    stmt3->lineno = 3;
    stmt3->class = RUN_STMT_CLASS;
    ret = prog_insert(stmt3);
    assert_success(ret);

    prog_reset(); //reset PC to just before first statement

    ret = prog_delete(2,3);
    assert_success(ret);
}

Test(program_suite, prog_delete_before_pc, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    STMT *stmt3 = malloc(sizeof(STMT));
    stmt3->lineno = 3;
    stmt3->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt3);
    assert_success(ret);

    ret = prog_delete(1,2);
    assert_success(ret);
}

Test(program_suite, prog_delete_at_pc, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    STMT *stmt3 = malloc(sizeof(STMT));
    stmt3->lineno = 3;
    stmt3->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt3);
    assert_success(ret);

    prog_goto(2); //places PC at statement 2

    ret = prog_delete(1,3);
    assert_success(ret);
}

Test(program_suite, prog_reset_fetch, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    prog_reset(); //PC points to statement 1
    STMT *stmt3 = prog_fetch(); 
    assert_values_equal((long)stmt1, (long)stmt3);
    free_stmt(stmt1);
    free_stmt(stmt2);
}

Test(program_suite, prog_fetch_empty, .timeout=2)
{
    STMT *stmt = prog_fetch();
    assert_values_equal((long)stmt, (long)NULL);
}

Test(program_suite, prog_fetch, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    prog_reset(); // PC points to statement 1
    prog_next(); // PC points to statement 2

    STMT *stmt3 = prog_fetch();
    assert_values_equal((long)stmt2, (long)stmt3);
    free_stmt(stmt1);
    free_stmt(stmt2);
}

Test(program_suite, prog_next_empty, .timeout=2)
{
    STMT *stmt = prog_next();
    assert_values_equal((long)stmt, (long)NULL);
}

Test(program_suite, prog_fetch_beyond, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);
    
    prog_reset();
    prog_next();

    //PC would be after the last statement. Nothing there.
    STMT *stmt = prog_next();
    assert_values_equal((long)stmt, (long)NULL);
    free_stmt(stmt1);
    free_stmt(stmt2);
}

Test(program_suite, prog_goto_valid, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt2 = malloc(sizeof(STMT));
    stmt2->lineno = 2;
    stmt2->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt2);
    assert_success(ret);

    STMT *stmt3 = malloc(sizeof(STMT));
    stmt3->lineno = 3;
    stmt3->class = LIST_STMT_CLASS;
    ret = prog_insert(stmt3);
    assert_success(ret);

    STMT *stmt = prog_goto(2);
    assert_values_equal((long)stmt, (long)stmt2);
    free_stmt(stmt1);
    free_stmt(stmt2);
    free_stmt(stmt3);
}

Test(program_suite, prog_goto_invalid, .timeout=2)
{
    STMT *stmt1 = malloc(sizeof(STMT));
    stmt1->lineno = 1;
    stmt1->class = RUN_STMT_CLASS;
    int ret = prog_insert(stmt1);
    assert_success(ret);

    STMT *stmt = prog_goto(2);
    assert_values_equal((long)stmt, (long)NULL);
    free_stmt(stmt1);
}
