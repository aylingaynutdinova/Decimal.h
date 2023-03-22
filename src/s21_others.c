#include "./s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int error = FAIL;
  if (result != NULL) {
    error = OK;
    int scale = get_scale(value);
    if (scale) {
      int sign = get_sign(value);
      value.bits[SCALE] = 0;
      while (scale > 0) {
        transit_div_10(value.bits);
        scale--;
      }
      set_sign(&value, sign);
    }
    copy_bits(value, result);
    result->bits[SCALE] = value.bits[SCALE];
  }
  return error;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int error = FAIL;
  if (result != NULL) {
    s21_zeroing_dec(result);
    error = OK;
    s21_decimal one = {{1, 0, 0, 0}};
    int scale = get_scale(value);
    int sign = get_sign(value);
    value.bits[SCALE] = 0;
    if (scale && s21_is_greater_or_equal(value, one)) {
      for (int i = 0; i < scale; i++) {
        transit_div_10(value.bits);
      }
      if (sign && scale < 29)
        s21_add(value, one, result);
      else
        copy_bits(value, result);
    } else {
      copy_bits(value, result);
    }
    set_sign(result, sign);
  }
  return error;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int error = FAIL;
  if (result != NULL) {
    s21_zeroing_dec(result);
    int scale = get_scale(value);
    int sign = get_sign(value);
    error = 0;
    if (scale) {
      s21_decimal five = {{5, 0, 0, 65536}};  // scale = 1
      s21_decimal one = {{1, 0, 0, 0}};
      s21_decimal dec = {{0, 0, 0, 0}};
      set_sign(&value, 0);
      error = s21_truncate(value, result);
      error = s21_sub(value, *result, &dec);
      if (is_odd(*result)) {
        if (s21_is_greater_or_equal(dec, five) == 1)
          s21_add(*result, one, result);
      } else {
        if (s21_is_greater(dec, five) == 1) s21_add(*result, one, result);
      }
    } else {
      copy_bits(value, result);
    }
    set_sign(result, sign);
  }
  return error;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  s21_decimal bleach = {0};
  *result = bleach;
  (*result).bits[0] = value.bits[0];
  (*result).bits[1] = value.bits[1];
  (*result).bits[2] = value.bits[2];
  (*result).bits[3] = value.bits[3];
  if (get_sign(*result))
    set_sign(result, 0);
  else
    set_sign(result, 1);
  return 0;
}
