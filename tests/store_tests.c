#include "__grading_helpers.h"

Test(store_suite, store_set_string_new, .timeout=2)
{
    char *var = "x";
    char *val = "56";
    int ret = store_set_string(var, val);
    char *out = store_get_string(var);
    assert_success(ret);
    assert_string_matches(out, val);
}

Test(store_suite, store_set_string_replace, .timeout=2)
{
    char *var = "x";
    char *val_1 = "60";
    char *val_2 = "70";
    int ret = store_set_string(var, val_1);
    assert_success(ret);
    ret = store_set_string(var, val_2);
    assert_success(ret);
    char *out = store_get_string(var);
    assert_success(ret);
    assert_string_matches(out, val_2);
}

Test(store_suite, store_set_string_delete, .timeout=2)
{
    char *var = "x";
    char *val_1 = "60";
    char *val_2 = NULL;
    int ret = store_set_string(var, val_1);
    assert_success(ret);
    ret = store_set_string(var, val_2);
    assert_success(ret);
    char *out = store_get_string(var);
    assert_success(ret);
    assert_null(out);
}

Test(store_suite, store_set_int_new, .timeout=2)
{
    char *var = "x";
    long val = 50;
    long out;
    int ret = store_set_int(var, val);
    assert_success(ret);
    ret = store_get_int(var, &out);
    assert_success(ret);
    assert_values_equal(out, val);
}

Test(store_suite, store_set_int_replace, .timeout=2)
{
    char *var = "x";
    long val = 50;
    long out;
    int ret = store_set_int(var, val);
    assert_success(ret);
    val = 60;
    ret = store_set_int(var, val);
    assert_success(ret);
    ret = store_get_int(var, &out);
    assert_success(ret);
    assert_values_equal(out, val);
}

Test(store_suite, store_get_string_non_exist, .timeout=2)
{
    char *var1 = "x";
    char *var2 = "y";
    char *val = "56";
    int ret = store_set_string(var1, val);
    assert_success(ret);
    char *out = store_get_string(var2);
    assert_null(out);
}

Test(store_suite, store_get_int_non_exist, .timeout=2)
{
    char *var1 = "x";
    char *var2 = "y";
    long val = 50;
    long out;
    int ret = store_set_int(var1, val);
    assert_success(ret);
    ret = store_get_int(var2, &out);
    assert_failure(ret);
}

Test(store_suite, store_get_int_from_string, .timeout=2)
{
    char *var1 = "x";
    char *val = "21";
    long out;
    int ret = store_set_string(var1, val);
    assert_success(ret);
    ret = store_get_int(var1, &out);
    assert_success(ret);
}

Test(store_suite, store_get_int_invalid, .timeout=2)
{
    char *var1 = "x";
    char *val = "inval";
    long out;
    int ret = store_set_string(var1, val);
    assert_success(ret);
    ret = store_get_int(var1, &out);
    assert_failure(ret);
}
