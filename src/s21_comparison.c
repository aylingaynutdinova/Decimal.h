#include "s21_decimal.h"

int s21_is_equal(s21_decimal a, s21_decimal b) {
  s21_normalize(&a, &b);
  int flag = 1;  // true
  for (int i = 0; i < 3 && flag; i++) {
    if ((a.bits[i] ^ b.bits[i])) flag = 0;
  }
  return flag;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  int flag = 0;
  if (s21_is_equal(a, b) == 0) flag = 1;
  return flag;
}

int s21_is_less(s21_decimal a, s21_decimal b) {
  int flag = 0;  // false
  int perepolnenie = s21_normalize(&a, &b);
  if ((perepolnenie) && (get_scale(a) > get_scale(b))) flag = 1;
  if (!perepolnenie) {
    if (get_sign(a) > get_sign(b))
      flag = 1;  // true;
    else if (get_sign(a) == get_sign(b)) {
      int i = 96;
      while (get_bit(a, i) == get_bit(b, i) && i != 0) i--;
      if (get_bit(a, i) < get_bit(b, i)) flag = 1;
      if (get_sign(a)) flag = !flag;
    }
    if (is_zero_new(a) && is_zero_new(b)) flag = 0;
  }
  return flag;
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  int flag = 0;
  int perepolnenie = s21_normalize(&a, &b);
  if ((perepolnenie) && (get_scale(a) < get_scale(b))) flag = 1;
  if (!perepolnenie) {
    if (!s21_is_equal(a, b)) {
      if (s21_is_less(a, b) == 0) flag = 1;
      if (is_zero_new(a) && is_zero_new(b)) flag = 0;
    }
  }
  return flag;
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  int flag = 0;
  if ((s21_is_greater(a, b)) || (s21_is_equal(a, b))) flag = 1;
  return flag;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  int flag = 0;
  if ((s21_is_less(a, b) == 1) || (s21_is_equal(a, b) == 1)) flag = 1;
  return flag;
}
