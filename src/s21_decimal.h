#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

/******************************************************************************/
/* Part 1. Implementation of the decimal.h library functions                  */
/******************************************************************************/

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

enum { LOW, MID, HIGH, SCALE };

typedef struct {
  unsigned int bits[4];
} s21_decimal;

#define s21_INF 1.0 / 0.0
#define s21_NAN 0.0 / 0.0
#define MAX_INT_BITS 32
#define END_OF_EXP 30
#define END_OF_MANT 22
#define BIT_23_MASK 0x400000
#define LAST_BIT_MANTISSA 95
#define SIGN_BIT 127
#define MINUS 2147483648
#define LAST_7_DIGITS_NUM 9999999
#define OK 0
#define FAIL 1
#define S21_FLT_MAX 79228162514264337593543950335.0F
#define is_large(src) (fabsf(src) >= S21_FLT_MAX)
#define DECIMAL_MAX(value)                                         \
  ((value.bits[0] == 0xFFFFFFFF && value.bits[1] == 0xFFFFFFFF) && \
   value.bits[2] == 0xFFFFFFFF)
#define DECIMAL_ZERO_SOLO(value) \
  ((!value.bits[0] && !value.bits[1]) && !value.bits[2])
#define DECIMAL_ZERO_MULTI(value1, value2)                      \
  (((!value1.bits[0] && !value1.bits[1]) && !value1.bits[2]) && \
   ((!value2.bits[0] && !value2.bits[1]) && !value2.bits[2]))

/* Arithmetic Operators */

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/* Comparison Operators */

int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);

/* Convertors and parsers */

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

/* Another functions */

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

/* Utilities */
uint32_t get_exp(float f);
int get_bit_from_num(uint32_t num, int index);
void set_bit_to_num(uint32_t *value, int index);
void set_null_dec(s21_decimal *value);
void s21_small_add(s21_decimal value_1, s21_decimal value_2,
                   s21_decimal *result);
void shift(s21_decimal *value, int index);
int get_last_bit(s21_decimal *value);
void s21_zeroing_dec(s21_decimal *value);
int s21_mul_10(s21_decimal *value);
int get_bit(s21_decimal value, int index);
void set_bit(s21_decimal *value, int index, int bit);
void print_bits(s21_decimal value);
void inverse_bit(s21_decimal *value, int index);
void shift_left(s21_decimal value, int position, s21_decimal *shifted);
void sub_itself(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void set_scale(s21_decimal *value, int scale);
int is_zero(s21_decimal value);
s21_decimal our_div(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *result);
int real_bits_amount(s21_decimal value);
void copy_bits(s21_decimal src, s21_decimal *dst);
int last_real_bit(s21_decimal value);
int s21_normalize(s21_decimal *value_1, s21_decimal *value_2);
void shift_right(s21_decimal *number, int i);
int real_bits_amount(s21_decimal value);
void s21_small_add(s21_decimal value_1, s21_decimal value_2,
                   s21_decimal *result);
void set_sign(s21_decimal *value, int bit);
int get_sign(s21_decimal value);
s21_decimal dec_fabs(s21_decimal *value);
int positive_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void negative_add(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result);
int get_scale(s21_decimal value);
int is_max_pow(s21_decimal value);
void print_bin_decimal(s21_decimal value);
void transit_div_10(unsigned int *bits);
int is_odd(s21_decimal bits);
int is_zero_new(s21_decimal value_1);
int scale_calc(s21_decimal remainder_scale, s21_decimal value_2,
               s21_decimal *result, int diff_scale);
void s21_div_10(s21_decimal *value);
int last_bit(s21_decimal value);
s21_decimal fabs_dec(s21_decimal value);

#endif  // SRC_S21_DECIMAL_H_
