// C standars library
#include <stdbool.h> //< bool
#include <stdint.h>  //< *int*_t
#include <stdio.h>   //< print

// Unit tests framework
// See : http://www.throwtheswitch.org/unity
#include "unity.h"

// Library under test
#include "strsepf.h"

//-----------------------------------------------------------
//
// Setup and teardown
//
//-----------------------------------------------------------
void
setUp(void)
{}

void
tearDown(void)
{}

//-----------------------------------------------------------
//
// Simple tests
//
//-----------------------------------------------------------
void
test_strsepf_simple_percent_d()
{
    char              test[] = "14";
    char const* const format = "%d";

    uint32_t answer = 3;
    int16_t  n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(14, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_u()
{
    char              test[] = "14";
    char const* const format = "%u";

    uint32_t answer = 3;
    int16_t  n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(14, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_i()
{
    char              test[] = "14";
    char const* const format = "%u";

    uint32_t answer = 3;
    int16_t  n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(14, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_x()
{
    char              test[] = "A";
    char const* const format = "%x";

    uint32_t answer = 3;
    int16_t  n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(10, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_b()
{
    char              test[] = "0110";
    char const* const format = "%b";

    uint32_t answer = 3;
    int16_t  n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(6, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_o()
{
    char              test[] = "56";
    char const* const format = "%o";

    uint32_t answer = 3;
    int16_t  n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(46, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_s()
{
    char              test[] = "allo";
    char const* const format = "%s";

    char*   answer = 0;
    int16_t n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL_STRING(test, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_d_negative()
{
    char              test[] = "-14";
    char const* const format = "%d";

    int32_t answer = 3;
    int16_t n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(-14, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_percent_x_with_prefix()
{
    char              test[] = "0xA";
    char const* const format = "%x";

    uint32_t answer = 3;
    int16_t  n = strsepf(test, format, &answer);

    TEST_ASSERT_EQUAL(10, answer);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_simple_number_is_too_big()
{
    char              test[] = "-> 923485709342875093248750923847509238475";
    char const* const format = "-> %d";

    uint32_t answer0 = 0;
    int16_t  n = strsepf(test, format, &answer0);
    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE, n);
}

void
test_strsepf_simple_number_is_too_small()
{
    char              test[] = "-> -923485709342875093248750923847509238475";
    char const* const format = "-> %d";

    uint32_t answer0 = 0;
    int16_t  n = strsepf(test, format, &answer0);
    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE, n);
}

//-----------------------------------------------------------
//
// Sub-specifier tests
//
//-----------------------------------------------------------
void
test_strsepf_subspecfier_width()
{
    char              test[] = "192.168.0.13";
    char const* const format = "%3d.%3d.%3d.%3d";

    uint32_t answer0 = 0;
    uint32_t answer1 = 0;
    uint32_t answer2 = 0;
    uint32_t answer3 = 0;
    int16_t  n = strsepf(test, format, &answer0, &answer1, &answer2, &answer3);

    TEST_ASSERT_EQUAL(4, n);
    TEST_ASSERT_EQUAL(192, answer0);
    TEST_ASSERT_EQUAL(168, answer1);
    TEST_ASSERT_EQUAL(0, answer2);
    TEST_ASSERT_EQUAL(13, answer3);
}

void
test_strsepf_subspecfier_with_too_big()
{
    char              test[] = "192.1684.0.13";
    char const* const format = "%3d.%3d.%3d.%3d";

    uint32_t answer0 = 0;
    uint32_t answer1 = 0;
    uint32_t answer2 = 0;
    uint32_t answer3 = 0;
    int16_t  n = strsepf(test, format, &answer0, &answer1, &answer2, &answer3);

    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_TOKEN_IS_BIGGER_THAN_WIDTH, n); //< 4-1
    TEST_ASSERT_EQUAL(192, answer0);
    TEST_ASSERT_EQUAL(0, answer1); //< failed to capture
    TEST_ASSERT_EQUAL(0, answer2);
}

void
test_strsepf_subspecfier_star()
{
    char              test[] = "1. my address is 890 rue de la rose, CAN.";
    char const* const format = "%*d. %*s %*s %*s %d %s, %*s";

    uint32_t answer0 = 0;
    char*    answer1 = 0;
    int16_t  n = strsepf(test, format, &answer0, &answer1);

    TEST_ASSERT_EQUAL(890, answer0);
    TEST_ASSERT_EQUAL_STRING("rue de la rose", answer1);
    TEST_ASSERT_EQUAL(2, n);
}

//-----------------------------------------------------------
//
// Complex tests
//
//-----------------------------------------------------------
void
test_strsepf_ip_address()
{
    char              test[] = "192.168.0.13";
    char const* const format = "%d.%d.%d.%d";

    uint32_t answer0 = 0;
    uint32_t answer1 = 0;
    uint32_t answer2 = 0;
    uint32_t answer3 = 0;
    int16_t  n = strsepf(test, format, &answer0, &answer1, &answer2, &answer3);

    TEST_ASSERT_EQUAL(192, answer0);
    TEST_ASSERT_EQUAL(168, answer1);
    TEST_ASSERT_EQUAL(0, answer2);
    TEST_ASSERT_EQUAL(13, answer3);
    TEST_ASSERT_EQUAL(4, n);
}

void
test_strsepf_commas_separeted_str()
{
    char              test[] = "+CMGL: 1,\"REC UNREAD\",\"+85291234567\",,\"06/11/11,00:30:29+32\"";
    char const* const format = "%s,%s,%s,%s,%s,%s";

    char* answer0 = NULL;
    char* answer1 = NULL;
    char* answer2 = NULL;
    char* answer3 = NULL;
    char* answer4 = NULL;
    char* answer5 = NULL;

    int16_t n = strsepf(test, format, &answer0, &answer1, &answer2, &answer3, &answer4, &answer5);

    TEST_ASSERT_EQUAL_STRING("+CMGL: 1", answer0);
    TEST_ASSERT_EQUAL_STRING("\"REC UNREAD\"", answer1);
    TEST_ASSERT_EQUAL_STRING("\"+85291234567\"", answer2);
    TEST_ASSERT_EQUAL_STRING("", answer3);
    TEST_ASSERT_EQUAL_STRING("\"06/11/11", answer4);
    TEST_ASSERT_EQUAL_STRING("00:30:29+32\"", answer5);
    TEST_ASSERT_EQUAL(6, n);
}

void
test_strsepf_select_substring()
{
    char              test[] = "$GPBWC,081837,,,,,,T,,M,,N,*13";
    char const* const format = "$%*sBWC,%d,%*s,%*s,%*s,%*s,%*s,%s,";

    uint32_t answer0 = 0;
    char*    answer1 = NULL;
    int16_t  n = strsepf(test, format, &answer0, &answer1);

    TEST_ASSERT_EQUAL(81837, answer0);
    TEST_ASSERT_EQUAL_STRING("T", answer1);
    TEST_ASSERT_EQUAL(2, n);
}

//-----------------------------------------------------------
//
// Special cases tests
//
//-----------------------------------------------------------

void
test_strsepf_empty_str()
{
    char              test[] = "";
    char const* const format = "%d,%s";

    uint32_t answer0 = 0;
    char*    answer1 = NULL;
    int16_t  n = strsepf(test, format, &answer0, &answer1);

    TEST_ASSERT_EQUAL(0, n);
}

void
test_strsepf_empty_fmt()
{
    char              test[] = "$GPBWC,081837,,,,,,T,,M,,N,*13";
    char const* const format = "";

    uint32_t answer0 = 0;
    char*    answer1 = NULL;
    int16_t  n = strsepf(test, format, &answer0, &answer1);

    TEST_ASSERT_EQUAL(0, n);
}

void
test_strsepf_percent_percent()
{
    char              test[] = "67/100% is your result.";
    char const* const format = "%d/100%% is your result.";

    uint32_t answer0 = 0;
    uint32_t answer1 = 0;
    int16_t  n = strsepf(test, format, &answer0, &answer1);
    TEST_ASSERT_EQUAL(1, n);
}

void
test_strsepf_percent_percent_str_percent_percent()
{
    char              test[] = "%%%a%%%";
    char const* const format = "%%%%%%%s%%%%%%";

    char*   answer0;
    int16_t n = strsepf(test, format, &answer0);
    TEST_ASSERT_EQUAL(1, n);
    TEST_ASSERT_EQUAL_STRING("a", answer0);
}

void
test_strsepf_null_arg()
{
    char              test[] = "51,area";
    char const* const format = "%d,%s";

    int16_t n = strsepf(test, format, NULL, NULL);
    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_INVALID_ARGS, n);
}

void
test_strsepf_invalid_format_specifier_doesnt_exist()
{
    char              test[] = "51,area";
    char const* const format = "%j";

    int16_t n = strsepf(test, format, NULL, NULL);
    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_INVALID_FORMAT, n);
}

void
test_strsepf_invalid_format_percent_with_notting()
{
    char              test[] = "51,area";
    char const* const format = "5% ";

    int16_t n = strsepf(test, format, NULL, NULL);
    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_INVALID_FORMAT, n);
}

void
test_strsepf_invalid_format_percent_at_the_end()
{
    char              test[] = "51,area";
    char const* const format = "51,%";

    int16_t n = strsepf(test, format, NULL, NULL);
    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_INVALID_FORMAT, n);
}

void
test_strsepf_invalid_format_no_separator()
{
    char              test[] = "sometext42";
    char const* const format = "%s%d"; //< cant do that

    int16_t n = strsepf(test, format, NULL, NULL);
    TEST_ASSERT_EQUAL(STRSEPF_RESULT_ERR_INVALID_ARGS, n);
}

//-----------------------------------------------------------
//
// Test bench
//
//-----------------------------------------------------------
int
main(void)
{
    UNITY_BEGIN();

    // Simple
    RUN_TEST(test_strsepf_simple_percent_d);
    RUN_TEST(test_strsepf_simple_percent_u);
    RUN_TEST(test_strsepf_simple_percent_i);
    RUN_TEST(test_strsepf_simple_percent_x);
    RUN_TEST(test_strsepf_simple_percent_b);
    RUN_TEST(test_strsepf_simple_percent_o);
    RUN_TEST(test_strsepf_simple_percent_s);
    RUN_TEST(test_strsepf_simple_percent_d_negative);
    RUN_TEST(test_strsepf_simple_percent_x_with_prefix);
    RUN_TEST(test_strsepf_simple_number_is_too_big);
    RUN_TEST(test_strsepf_simple_number_is_too_small);

    // Sub-specifier
    RUN_TEST(test_strsepf_subspecfier_width);
    RUN_TEST(test_strsepf_subspecfier_with_too_big);
    RUN_TEST(test_strsepf_subspecfier_star);

    // Complex
    RUN_TEST(test_strsepf_ip_address);
    RUN_TEST(test_strsepf_commas_separeted_str);
    RUN_TEST(test_strsepf_select_substring);

    // Special cases
    RUN_TEST(test_strsepf_empty_str);
    RUN_TEST(test_strsepf_empty_fmt);
    RUN_TEST(test_strsepf_percent_percent);
    RUN_TEST(test_strsepf_percent_percent_str_percent_percent);
    RUN_TEST(test_strsepf_null_arg);
    RUN_TEST(test_strsepf_invalid_format_specifier_doesnt_exist);
    RUN_TEST(test_strsepf_invalid_format_percent_with_notting);
    RUN_TEST(test_strsepf_invalid_format_percent_at_the_end);
    RUN_TEST(test_strsepf_invalid_format_no_separator);

    return UNITY_END();
}
