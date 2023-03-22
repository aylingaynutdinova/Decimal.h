// Arithmetic Operators
#include "s21_decimal.h"

s21_decimal fabs_dec(s21_decimal value) {
  s21_decimal fabs_value = value;
  set_sign(&fabs_value, 0);
  return fabs_value;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_normalize(&value_1, &value_2);
  s21_zeroing_dec(result);
  int flag = 0;
  int temp = 0;
  s21_decimal fabs_value1 = fabs_dec(value_1);
  s21_decimal fabs_value2 = fabs_dec(value_2);
  if ((DECIMAL_MAX(value_1) && DECIMAL_MAX(value_2)) &&
      (get_sign(value_1) != get_sign(value_2)))
    flag = 0;
  else if ((DECIMAL_MAX(value_1) || DECIMAL_MAX(value_2)) &&
           (get_sign(value_1) == get_sign(value_2))) {
    if (get_sign(value_1) == 1)
      flag = 2;
    else
      flag = 1;
  } else {
    // оба отриц или положит
    if (get_bit(value_1, 127) == get_bit(value_2, 127)) {
      for (int i = 0; i < 96; i++) {
        int res = get_bit(value_1, i) + get_bit(value_2, i) + temp;
        if (temp != 0) temp = 0;
        if (res == 1)
          set_bit(result, i, 1);
        else if (res == 2) {
          set_bit(result, i, 0);
          temp++;
        } else if (res == 3) {
          temp++;
          set_bit(result, i, 1);
        }
      }
      // проверка на переполнениe
      if (get_bit(*result, 95) && temp) {
        if (get_bit(value_1, 127))
          flag = 2;
        else
          flag = 1;
      }
      if ((get_bit(value_1, 127)) && !flag) set_bit(result, 127, 1);
    }
    // если одно из них отрицательное
    else if (get_bit(value_1, 127) || get_bit(value_2, 127)) {
      if (s21_is_greater(fabs_value1, fabs_value2)) {
        sub_itself(fabs_value1, fabs_value2, result);
        set_sign(result, get_sign(value_1));
      } else if (!s21_is_greater(fabs_value1, fabs_value2)) {
        sub_itself(fabs_value2, fabs_value1, result);
        set_sign(result, get_sign(value_2));
      }
    }
    set_scale(result, get_scale(value_1));
    //если оба отрицательные
  }
  if (flag) s21_zeroing_dec(result);
  return flag;
}

void sub_itself(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_zeroing_dec(result);
  int remainder = 0;
  for (int i = 0; i <= last_bit(value_1); i++) {
    int bit_value1 = get_bit(value_1, i);
    int bit_value2 = get_bit(value_2, i);
    if (!bit_value1 && !bit_value2) {
      if (remainder) {
        set_bit(result, i, 1);
        remainder = 1;
      }
    } else if (bit_value1 && !bit_value2) {
      if (remainder)
        remainder = 0;
      else
        set_bit(result, i, 1);
    } else if (!bit_value1) {
      if (!remainder) set_bit(result, i, 1);
      remainder = 1;
    } else if (remainder) {
      remainder = 1;
      set_bit(result, i, 1);
    }
  }
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_normalize(&value_1, &value_2);
  s21_zeroing_dec(result);
  s21_decimal fabs_value_1 = fabs_dec(value_1);
  s21_decimal fabs_value_2 = fabs_dec(value_2);
  int flag = 0;
  if (get_sign(value_1) != get_sign(value_2)) {
    if ((DECIMAL_MAX(value_1) && get_sign(value_1)) ||
        (DECIMAL_MAX(value_2) && get_sign(value_1))) {
      flag = 2;
      s21_zeroing_dec(result);
    } else if ((DECIMAL_MAX(value_1) && get_sign(value_2)) ||
               (DECIMAL_MAX(value_2) && get_sign(value_2))) {
      flag = 1;
      s21_zeroing_dec(result);
    } else {
      s21_add(fabs_value_1, fabs_value_2, result);
      set_sign(result, get_sign(value_1));
    }
  } else {
    if (s21_is_greater(fabs_value_1, fabs_value_2)) {
      sub_itself(fabs_value_1, fabs_value_2, result);
      set_sign(result, get_sign(value_1));
    } else if (s21_is_less(fabs_value_1, fabs_value_2)) {
      sub_itself(fabs_value_2, fabs_value_1, result);
      int sign = !get_sign(value_2);
      set_sign(result, sign);
    }
    set_scale(result, get_scale(value_1));
  }
  return flag;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_zeroing_dec(result);
  int flag = 0;
  if (DECIMAL_ZERO_SOLO(value_1) || DECIMAL_ZERO_SOLO(value_2))
    flag = 0;
  else if (DECIMAL_MAX(value_1) || DECIMAL_MAX(value_2)) {
    if (get_sign(value_1) == get_sign(value_2))
      flag = 1;
    else
      flag = 2;
  } else {
    s21_decimal temp = {{0, 0, 0, 0}};
    for (int i = 0; i < 96; i++) {
      for (int j = 0; j < 96; j++) {
        if (get_bit(value_2, i)) {
          set_bit(&temp, j, get_bit(value_1, j));
        } else {
          set_bit(&temp, j, 0);
        }
      }
      if (i) shift_left(temp, i, &temp);
      s21_add(*result, temp, result);
    }
    int scale = get_scale(value_1) + get_scale(value_2);
    set_scale(result, scale);
  }
  return flag;
}

s21_decimal our_div(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *result) {
  s21_zeroing_dec(result);
  s21_decimal const zero = {0};
  s21_decimal remainder = value_1;
  int old_real_bits = real_bits_amount(value_2);
  shift_left(value_2, 95 - old_real_bits, &value_2);
  while ((s21_is_not_equal(value_2, zero)) && real_bits_amount(*result) < 96) {
    shift_left(*result, 1, result);
    if (s21_is_greater_or_equal(remainder, value_2)) {
      set_bit(result, 0, 1);
      s21_sub(remainder, value_2, &remainder);
    }
    if (real_bits_amount(value_2) == old_real_bits) break;
    shift_right(&value_2, 1);
  }
  return remainder;
}
void s21_div_10(s21_decimal *value) {
  s21_decimal ten = {0};
  our_div(*value, ten, value);
}
int scale_calc(s21_decimal remainder_scale, s21_decimal value_2,
               s21_decimal *result, int diff_scale) {
  int num_scale = 0;
  s21_decimal zero = {0};
  while (!s21_is_equal(remainder_scale, zero) && num_scale < 28) {
    s21_mul_10(&remainder_scale);
    remainder_scale = our_div(remainder_scale, value_2, &remainder_scale);
    num_scale++;
  }
  int scale = diff_scale + num_scale;
  while (scale > 28) {
    s21_div_10(result);
    scale--;
  }
  while (scale < 0) {
    s21_mul_10(result);
    scale++;
  }
  return scale;
}

// scale is calculated
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_zeroing_dec(result);
  int diff_scale = get_scale(value_1) - get_scale(value_2);
  int flag = 0;
  if (is_zero(value_2)) flag = 3;  // division by 0
  if (!flag) {
    s21_decimal fraction_part = {0};
    s21_decimal remainder = our_div(value_1, value_2, result);
    s21_decimal remainder_scale = remainder;
    int res_count = 0;
    int res_count_old = 0;
    int res = 0;
    int max_value = is_zero(*result) ? 95 : 96;
    if (!is_zero(remainder)) {
      while (!is_zero(remainder) && res_count_old + res < max_value) {
        res_count = real_bits_amount(*result);
        s21_mul_10(result);
        s21_mul_10(&remainder);
        remainder = our_div(remainder, value_2, &fraction_part);
        s21_add(*result, fraction_part, result);
        res_count_old = real_bits_amount(*result);
        res = res_count_old - res_count;
      }
    }
    int scale = scale_calc(remainder_scale, value_2, result, diff_scale);
    set_scale(result, scale);
  }
  return flag;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag = 0;
  s21_zeroing_dec(result);
  s21_decimal result_div = {0};
  *result = our_div(value_1, value_2, &result_div);
  if (get_sign(value_1)) set_sign(result, 1);
  return flag;
}
