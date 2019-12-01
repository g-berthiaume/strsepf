/* +------------------------------------------------------+
 * | strsepf.h                                            |
 * | String parsing utilities combining the memory safety |
 * | of strsep the convenience of a sscanf-like           |
 * | interface.                                           |
 * |                                                      |
 * +------------------------------------------------------+
 * |                                        G. Berthiaume |
 * |                                          MIT licence |
 * |                                                 2019 |
 * +------------------------------------------------------+
 */
#pragma once

//-------------------------------------------//
//                                           //
//                Includes                   //
//                                           //
//-------------------------------------------//
#include <ctype.h>   //< cstdlib : isdigit
#include <errno.h>   //< cstdlib : For strtol_s
#include <stdarg.h>  //< cstdlib : va_list, va_start, va_arg, va_end
#include <stdbool.h> //< cstdlib : bool
#include <stdint.h>  //< cstdlib : *int*_t, limit
#include <stdlib.h>  //< cstdlib : strtol
#include <string.h>  //< cstdlib : strchr

// NOTE:
// `strsep` is not a POSIX or Linux standard function, it's a BSD function. Therefore, there's no
// garantiy it will be included in `string.h`.
// We can include it manually:
#include "strsep.h"

//-------------------------------------------//
//                                           //
//             Definitions                   //
//                                           //
//-------------------------------------------//

/*
 * Enumerates all the possible errors for the strsepf function.
 * All non-negative number are not an error.
 */
typedef enum
{
    // Number conversion error
    STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE = -8,
    STRSEPF_RESULT_ERR_STRTOI_EXTRA_CHAR = -7,
    STRSEPF_RESULT_ERR_STRTOI_NOT_A_DECIMAL = -6,
    // Optional specifier error
    STRSEPF_RESULT_ERR_TOKEN_IS_BIGGER_THAN_WIDTH = -5,
    // General error
    STRSEPF_RESULT_ERR_INPUT_DOESNT_MATCH_FORMAT = -4,
    STRSEPF_RESULT_ERR_INVALID_FORMAT = -3,
    STRSEPF_RESULT_ERR_INVALID_ARGS = -2,
    STRSEPF_RESULT_ERR_INVALID_PARAMETER = -1,
    // Success
    STRSEPF_RESULT_OK = 0,
} strsepf_result;

//-------------------------------------------//
//                                           //
//               Interface                   //
//                                           //
//-------------------------------------------//

int16_t
strsepf(char mutStr[], char const* fmt, ...);

int16_t
vstrsepf(char mutStr[], char const* fmt, va_list arg);

//-------------------------------------------//
//                                           //
//           Helper functions                //
//                                           //
//-------------------------------------------//
uint32_t
strtou32_s(const char* buff, uint8_t base, strsepf_result* err);

int32_t
strto32_s(const char* buff, uint8_t base, strsepf_result* err);

//-------------------------------------------//
//                                           //
//              Implementation               //
//                                           //
//-------------------------------------------//

/*
 * `strsepf` is a wrapper to `vstrsepf`.
 * See the `vstrsepf` declaration for more information.
 */
int16_t
strsepf(char mutStr[], char const* fmt, ...)
{
    int16_t rc;
    va_list arg;
    va_start(arg, fmt);
    rc = vstrsepf(mutStr, fmt, arg);
    va_end(arg);
    return rc;
}

/*
 * `vstrsepf` is string parsing utility function born from
 * the idea of using `strsep` with a `sscanf`-like interface.
 *
 * It is designed to be a safer `scanf` for embedded system application.
 *
 * Some key design choices:
 * - Will destroy its input string (by adding '\0').
 * - No dynamic memory allocation
 * - No string copy.
 * - No floating point support.
 *
 *
 * ARGUMENTS:
 *  @param: mutStr - Mutable input string (will be destroyed).
 *  @param: fmt    - Format string.
 *  @param: arg    - Aguments lists (va_list).
 *
 *
 * RETURNS:
 *  Will return the number of parsed arguments or a negative number if the parsing
 *  failed.
 *  If number is negative, more information about the error can be found
 *
 * USAGE EXAMPLES:
 *
 *  eg 1 - Parse IP string
 *  ----------------------
 *
 *   char ip[] = "192.168.0.13";      //< Mutable string
 *
 *    uint8_t ip0 = 0;
 *    uint8_t ip1 = 0;
 *    uint8_t ip2 = 0;
 *    uint8_t ip3 = 0;
 *
 *    int16_t  n = strsepf(ip, "%3d.%3d.%3d.%3d", &ip0, &ip1, &ip2, &ip3);
 *
 *    TEST_ASSERT_EQUAL(4,    n);   //< Will pass.
 *    TEST_ASSERT_EQUAL(192,  ip0); //< Will pass.
 *    TEST_ASSERT_EQUAL(168,  ip1); //< Will pass.
 *    TEST_ASSERT_EQUAL(0,    ip2); //< Will pass.
 *    TEST_ASSERT_EQUAL(13,   ip3); //< Will pass.
 *
 *
 *  eg 2 - Parse NMEA (GPS) message
 *  -------------------------------
 *
 *    // For a $xxBWC message, we only want to get
 *    // - the 1st argument in number
 *    // - the 7th argument as a string
 *
 *    char               msg[] = "$GPBWC,081837,,,,,,T,,M,,N,*13";
 *    char const* const format = "$%*sBWC,%d,%*s,%*s,%*s,%*s,%*s,%s,";
 *
 *    uint32_t answer0 = 0;
 *    char*    answer1 = NULL;
 *    int16_t  n = strsepf(msg, format, &answer0, &answer1);
 *
 *    TEST_ASSERT_EQUAL(81837, answer0);       //< Will pass.
 *    TEST_ASSERT_EQUAL_STRING("T", answer1);  //< Will pass.
 *    TEST_ASSERT_EQUAL(2, n);                 //< Will pass.
 *
 *
 * FORMAT SPECIFIER:
 *
 *  | Specifier   | Descriptions                                                    |
 *  |-------------|-----------------------------------------------------------------|
 *  | %i, %d, %u, | Any number of digits                                            |
 *  | %o          | Any number of octal digits (0-7)                                |
 *  | %x          | Any number of hexadecimal digits (0-9, a-f, A-F*)               |
 *  | %%          | A % followed by another % matches a single %.                   |
 *  | %s          | A string with any character in it. A terminating null character |
 *  |             | is automatically added at the end of the stored sequence.#      |
 *
 * FORMAT OPTIONAL SPECIFIER:
 *
 *  | O Specifier | Descriptions                                                    |
 *  |-------------|-----------------------------------------------------------------|
 *  | *           | An optional starting asterisk indicates that the data is to be  |
 *  |             | read from the stream but ignored.                               |
 *  | width       | Specifies the maximum number of characters to be read in the    |
 *  |             | current reading operation.                                      |
 *
 *
 *
 */
int16_t
vstrsepf(char mutStr[], char const* fmt, va_list arg)
{
    if (mutStr == NULL || fmt == NULL) {
        return STRSEPF_RESULT_ERR_INVALID_PARAMETER;
    }

#define SUPPORTED_SPECIFIER "dibouxs"

    int count = 0;
    while (*mutStr && *fmt) {

        if (*fmt == '%') {
            fmt++;

            if (*fmt == '%') {
                fmt++;
                mutStr++; //< `%%` is just the `%` character.
                continue;
            }

            // A format specifier follows this prototype: [=%[*][width][modifiers]type=]
            char     specifierType = '\0';
            bool     noAssignFlag = false;
            uint32_t width = 0;
            for (; (*fmt != '\0' && fmt != NULL); fmt++) {
                if (strchr(SUPPORTED_SPECIFIER, *fmt) != NULL) {
                    specifierType = *fmt;
                    fmt++;
                    break; //< Specifier type is always the last element of a specifier string

                } else if (*fmt == '*') {
                    noAssignFlag = true;

                } else if (*fmt >= '1' && *fmt <= '9') {
                    char const* tc;
                    for (tc = fmt; isdigit(*fmt); fmt++)
                        ;

                    strsepf_result err;
                    width = strtou32_s(tc, 10, &err);
                    if (err != STRSEPF_RESULT_ERR_STRTOI_EXTRA_CHAR) {
                        return STRSEPF_RESULT_ERR_INVALID_FORMAT; //< width shall be follow by a
                                                                  // specifier type
                    }
                    fmt--;
                } else {
                    return STRSEPF_RESULT_ERR_INVALID_FORMAT;
                }
            }

            if (specifierType == '\0') {
                return STRSEPF_RESULT_ERR_INVALID_FORMAT;
            }

            // Tokenisation
            char*      token;
            const bool continueUntilTheEnd = (*fmt == '\0');
            if (continueUntilTheEnd) {
                token = mutStr;

            } else {
                // NOTE:
                // If termination is not found in `mutStr`, will return the entier string
                char termination[] = { *fmt, '\0' };
                token = strsep(&mutStr, termination);
                if (token == NULL) {
                    return STRSEPF_RESULT_ERR_INPUT_DOESNT_MATCH_FORMAT; //< no token found
                }
                fmt++;
            }

            // Optinal specifier logic
            if (noAssignFlag) {
                continue; //< ignore it.
            }
            if (width > 0) {
                if (width < strlen(token)) {
                    return STRSEPF_RESULT_ERR_TOKEN_IS_BIGGER_THAN_WIDTH;
                }
            }

            // Scan string
            strsepf_result strtolErr = STRSEPF_RESULT_OK;
            if (specifierType == 's') {
                char** ptr = va_arg(arg, char**);
                if (ptr == NULL) {
                    return STRSEPF_RESULT_ERR_INVALID_ARGS;
                }
                *ptr = token;
                count++;
            }
            // Scan an unsigned int
            else if (strchr("uxob", specifierType) != NULL) {
                uint8_t base = 0;
                if (specifierType == 'u') {
                    base = 10;
                } else if (specifierType == 'x') {
                    base = 16;
                } else if (specifierType == 'o') {
                    base = 8;
                } else if (specifierType == 'b') {
                    base = 2;
                }
                uint32_t* ptr = va_arg(arg, uint32_t*);
                if (ptr == NULL) {
                    return STRSEPF_RESULT_ERR_INVALID_ARGS;
                }
                *ptr = strtou32_s(token, base, &strtolErr);
                if (strtolErr < STRSEPF_RESULT_OK) {
                    return strtolErr;
                }
                count++;
            }
            // Scan a signed int
            else if (strchr("di", specifierType) != NULL) {
                uint8_t  base = 10;
                int32_t* ptr = va_arg(arg, int32_t*);
                if (ptr == NULL) {
                    return STRSEPF_RESULT_ERR_INVALID_ARGS;
                }
                *ptr = strto32_s(token, base, &strtolErr);
                if (strtolErr < STRSEPF_RESULT_OK) {
                    return strtolErr;
                }
                count++;
            } else {
                return STRSEPF_RESULT_ERR_INVALID_FORMAT; //< Specifier not handled -> should not
                                                          // happen
            }

        } else { /* !(*fmt == '%') */
            if (*fmt != *mutStr) {
                return STRSEPF_RESULT_ERR_INPUT_DOESNT_MATCH_FORMAT;
            } else {
                fmt++;
                mutStr++;
            }
        } // END if (*fmt == '%')
    }     // END while (*mutStr && *fmt)
    return count;
}

//-------------------------------------------//
//                                           //
//    Helper functions Implemetation         //
//                                           //
//-------------------------------------------//

/*
 * String to uint32 utility
 */
uint32_t
strtou32_s(const char buff[], uint8_t base, strsepf_result* err)
{
    if (err == NULL) {
        return 0;
    }
    if (buff == NULL || base == 0) {
        *err = STRSEPF_RESULT_ERR_INVALID_PARAMETER;
        return 0;
    }

    char* end;
    errno = 0;

    const unsigned long ul = strtoul(buff, &end, base);
    if (end == buff) {
        *err = STRSEPF_RESULT_ERR_STRTOI_NOT_A_DECIMAL; //< not a decimal number

    } else if ('\0' != *end) {
        *err = STRSEPF_RESULT_ERR_STRTOI_EXTRA_CHAR; //< extra characters at end of input

    } else if (ERANGE == errno) {
        *err = STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE; //< out of range of type u long

    } else if (ul > UINT32_MAX) {
        *err = STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE; //< greater than MAX

    } else {
        *err = STRSEPF_RESULT_OK; //< ok
    }
    return (uint32_t)ul;
}

/*
 * String to int32 utility
 */
int32_t
strto32_s(const char buff[], uint8_t base, strsepf_result* err)
{
    if (err == NULL) {
        return 0;
    }
    if (buff == NULL || base == 0) {
        *err = STRSEPF_RESULT_ERR_INVALID_PARAMETER;
        return 0;
    }

    char* end;
    errno = 0;

    const long sl = strtoul(buff, &end, base);
    if (end == buff) {
        *err = STRSEPF_RESULT_ERR_STRTOI_NOT_A_DECIMAL; //< not a decimal number

    } else if ('\0' != *end) {
        *err = STRSEPF_RESULT_ERR_STRTOI_EXTRA_CHAR; //< extra characters at end of input

    } else if (ERANGE == errno) {
        *err = STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE; //< out of range of type u long

    } else if (sl > INT32_MAX) {
        *err = STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE; //< greater than MAX

    } else if (sl < INT32_MIN) {
        *err = STRSEPF_RESULT_ERR_STRTOI_OUT_OF_RANGE; //< less than MIN

    } else {
        *err = STRSEPF_RESULT_OK; //< ok
    }
    return (int32_t)sl;
}
