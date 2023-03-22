#include "./s21_decimal.h"

//* LITTLE ADDITIONAL FUNCS *//

// выдаст чему равен бит по индексу
int get_bit(s21_decimal value, int index) {
  int i = index / 32;
  return (value.bits[i] & (1 << (index - 32 * i))) ? 1 : 0;
}

// прирвнивает бит по индексу к желаемому значению
void set_bit(s21_decimal *value, int index, int bit) {
  int i = index / 32;
  if (bit == 1)
    value->bits[i] = value->bits[i] | (1 << (index - 32 * i));
  else
    value->bits[index / 32] = value->bits[index / 32] & ~(1 << (index % 32));
}

// поменять бит по индексу на противоположный ему знак
void inverse_bit(s21_decimal *value, int index) {
  int i = index / 32;
  value->bits[i] = value->bits[i] ^ (1 << (index - 32 * i));
}

void s21_zeroing_dec(s21_decimal *value) {
  for (int i = 0; i < 4; ++i) value->bits[i] = 0;
}

uint32_t get_exp(float f) {
  uint32_t bin_num = *((uint32_t *)&f);
  uint32_t result = 0;
  for (int i = END_OF_EXP; i < END_OF_MANT; i--) {
    if (get_bit_from_num(bin_num, i)) {
      set_bit_to_num(&result, i);
    }
  }
  return result - 127;
}

int is_zero(s21_decimal value) {
  int is_zero = 0;
  if (value.bits[0] == 0) is_zero = 1;
  return is_zero;
}

s21_decimal dec_fabs(s21_decimal *value) {
  s21_decimal fabs_value = *value;
  set_bit(&fabs_value, SIGN_BIT, 0);
  return fabs_value;
}

void shift_left(s21_decimal value, int position, s21_decimal *shifted) {
  s21_zeroing_dec(shifted);
  for (int i = 0; i < 96; i++) {
    if (i < position)
      set_bit(shifted, i, 0);
    else
      set_bit(shifted, i, get_bit(value, i - position));
  }
}

void set_scale(s21_decimal *value, int scale) {
  value->bits[SCALE] &= MINUS;
  scale <<= 16;
  value->bits[SCALE] |= scale;
}

int get_scale(s21_decimal value) {
  uint32_t mask = 127 << 16;
  int scale = value.bits[SCALE] & mask;
  return scale >> 16;
}

////
void copy_bits(s21_decimal src, s21_decimal *dst) {
  dst->bits[0] = src.bits[0];
  dst->bits[1] = src.bits[1];
  dst->bits[2] = src.bits[2];
  // dst->bits[3] = src.bits[3];
}

void set_sign(s21_decimal *value, int bit) { set_bit(value, SIGN_BIT, bit); }

int get_sign(s21_decimal value) { return get_bit(value, SIGN_BIT); }

int last_real_bit(s21_decimal value) {
  int last_b = LAST_BIT_MANTISSA;
  for (; last_b && get_bit(value, last_b); last_b--)
    ;
  return last_b;
}

void shift_right(s21_decimal *number, int i) {
  do {
    for (int j = 0; j < 96; j++) {
      set_bit(number, j, get_bit(*number, j + 1));
    }
    i--;
  } while (i > 0);
}

// maybe without +
int real_bits_amount(s21_decimal value) {
  int real_bits = 96;
  s21_decimal temp_value = value;
  while (!get_bit(value, real_bits)) {
    shift_left(temp_value, 1, &temp_value);
    real_bits--;
  }
  return real_bits + 1;
}

void s21_small_add(s21_decimal value_1, s21_decimal value_2,
                   s21_decimal *result) {
  int temp = 0;
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
}

int s21_mul_10(s21_decimal *value) {
  s21_decimal temp1 = {{0, 0, 0, 0}};
  s21_decimal temp2 = {{0, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};
  shift_left(*value, 1, &temp1);
  shift_left(*value, 3, &temp2);
  s21_small_add(temp1, temp2, &result);
  copy_bits(result, value);
  return 0;
}

int s21_normalize(s21_decimal *value_1, s21_decimal *value_2) {
  int error = 0;
  s21_decimal *min_value = NULL;
  s21_decimal *max_value = NULL;
  int scale_1 = get_scale(*value_1);
  int scale_2 = get_scale(*value_2);
  if (scale_1 < scale_2) {
    max_value = value_2;
    min_value = value_1;
  } else {
    max_value = value_1;
    min_value = value_2;
  }
  int max_scale = get_scale(*max_value);
  int min_scale = get_scale(*min_value);
  while (min_scale < max_scale && !error) {
    if (get_bit(*min_value, LAST_BIT_MANTISSA)) {
      error = 1;
    } else {
      s21_mul_10(min_value);
      min_scale++;
    }
  }
  if (!error) set_scale(min_value, min_scale);
  return error;
}

void set_bit_to_num(unsigned int *value, int index) {
  uint32_t mask = 1u << index;
  *value |= mask;
}

int get_bit_from_num(uint32_t num, int index) {
  uint32_t mask = 1u << index;
  return !!(num & mask);
}

int is_odd(s21_decimal bits) {
  uint64_t temp = 0, remainder = 0;
  int result = 0;
  for (int i = HIGH; i >= LOW; i--) {
    temp = (uint64_t)bits.bits[i] | remainder;
    remainder = (temp % 10) << MAX_INT_BITS;
    if (i == LOW) result = (remainder >> MAX_INT_BITS) % 2;
    bits.bits[i] = (uint32_t)(temp / 10);
  }
  return result;
}

void transit_div_10(unsigned int *bits) {
  /*добавляем слева остаток(разряд) к биту младше предыдущего
   берем остаток у транзитного значения и сдвигаем его на 32 бита, чтобы
   добавить к следующему биту записываем в i бит транзитное значение поделенное
   на 10 и приведенное к 32битовому типу, чтобы забрать разряд(необходимый
   двоичный код) справа*/
  uint64_t temp = 0, remainder = 0;
  for (int i = HIGH; i >= LOW; i--) {
    temp = (uint64_t)bits[i] | remainder;
    if (i > 0) remainder = (temp % 10) << MAX_INT_BITS;
    bits[i] = (uint32_t)(temp / 10);
  }
}

int is_zero_new(s21_decimal value_1) {
  s21_decimal const zero = {0};
  return s21_is_equal(value_1, zero);
}

int last_bit(s21_decimal value) {
  int last_b = LAST_BIT_MANTISSA;
  for (; last_b && !get_bit(value, last_b); last_b--)
    ;
  return last_b;
}
