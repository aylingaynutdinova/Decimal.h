#include "./s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int error = OK;
  if (dst != NULL) {
    s21_zeroing_dec(dst);
    if (src < 0) {
      set_sign(dst, 1);
      src *= -1;
    }
    if (!error) dst->bits[LOW] = (uint32_t)src;
  }
  return error;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error = OK;
  int sign = 0;
  int its_pretty_avg = (fabs(src) > 0 && fabs(src) < 1e-28) ? 1 : 0;
  if (its_pretty_avg || is_large(src) || src == s21_INF || src == -s21_INF ||
      isnan(src)) {
    error = FAIL;
    if (its_pretty_avg) {
      s21_zeroing_dec(dst);
      if (src < 0) set_sign(dst, 1);
    }
  } else {
    s21_zeroing_dec(dst);
    if (src < 0) {
      sign = 1;
      src *= -1;
    }
    int scale = 0;
    float temp_dec = 0;
    float int_pos = 0;
    int zero_count = 0;
    int significant = 6;
    if (src > LAST_7_DIGITS_NUM) {
      long double transit = src;
      while (transit > LAST_7_DIGITS_NUM) {
        transit = transit / 10;
        zero_count++;
      }
      src = transit;
    } else {
      while ((temp_dec = modff(src, &int_pos)) && scale < 28 &&
             significant > 0) {
        src *= 10;
        scale++;
        if (int_pos != 0) {
          significant--;
        }
      }
    }
    if (!zero_count) src = rintf(src);
    int bin_num = (int)src;
    for (int i = 0; i < END_OF_MANT + 1; i++) {
      if (get_bit_from_num(bin_num, i)) {
        set_bit(dst, i, 1);
      }
    }
    while (zero_count > 0) {
      s21_mul_10(dst);
      zero_count--;
    }
    set_scale(dst, scale);
    set_sign(dst, sign);
  }
  return error;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int error = OK;
  int sign = get_sign(src);
  if (src.bits[MID] || src.bits[HIGH] || dst == NULL ||
      (src.bits[0] > (((uint32_t)INT32_MAX) + sign))) {
    error = FAIL;
  } else {
    int scale = get_scale(src);
    while (scale > 0) {
      src.bits[LOW] /= 10;
      scale--;
    }
    *dst = src.bits[LOW];
    if (sign) {
      *dst = *dst * -1;
    }
  }
  return error;
}

int s21_from_decimal_to_float(s21_decimal value, float *num) {
  *num = 0;
  int error = OK;
  double transit = 0.0;
  for (int i = 0; i < 96; i++) {
    if (get_bit(value, i)) {
      transit += pow(2.0, i);
    }
  }
  int scale = get_scale(value);
  while (scale > 0) {
    transit = transit / 10;
    scale--;
  }
  *num = (float)transit;
  if (get_sign(value)) *num *= -1;
  return error;
}
