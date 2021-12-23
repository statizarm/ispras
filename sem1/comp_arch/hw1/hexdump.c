#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <immintrin.h>

typedef void (*hex_dump_foo) (char *restrict, unsigned char *restrict, size_t);

typedef struct {
  hex_dump_foo func;
  char *name;
} test_case;

void hex_dump_v1(char *restrict out, unsigned char *restrict in, size_t n);
void hex_dump_v2(char *restrict out, unsigned char *restrict in, size_t n);
void hex_dump_v3(char *restrict out, unsigned char *restrict in, size_t n);
void hex_dump_v4(char *restrict out, unsigned char *restrict in, size_t n);

test_case test_suite[] = {
  {
    .func = hex_dump_v1,
    .name = "version 1"
  },
  {
    .func = hex_dump_v2,
    .name = "version 2"
  },
  {
    .func = hex_dump_v3,
    .name = "version 3"
  },
  {
    .func = hex_dump_v4,
    .name = "version 4"
  }
};

char encoding[] = {
  '0', '1', '2', '3',
  '4', '5', '6', '7',
  '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F'
};

uint16_t table[256];

char source[1 << 20];
char result[1 << 22];

void hex_dump_v1(char *restrict out, unsigned char *restrict in, size_t n) {
  for (int i = 0; i < n; ++i) {
    int k = i << 2;
    *(uint32_t *) &out[k] = (uint32_t)0x0000785c | (uint32_t)((uint32_t) encoding[in[i] & 15] << 24) | (uint32_t)((uint32_t) encoding[in[i] >> 4] << 16);
  }

  out[(n + 1) << 2] = '\0';
}

void hex_dump_v2(char *restrict out, unsigned char *restrict in, size_t n) {
  unsigned char *restrict end = in + n;
  for (;in < end; out += 4, ++in) {
    *(uint32_t *) out = 0x0000785c | (table[*in] << 16);
  }

  *out = '\0';
}

void hex_dump_v3(char *restrict out, unsigned char *restrict in, size_t n) {
#define UNROLL_DEGREE 8 // must be power of two
  unsigned char *restrict end = in + (n & ~(UNROLL_DEGREE - 1));
  uint32_t syms[UNROLL_DEGREE];

  for (; in < end; in += UNROLL_DEGREE, out += UNROLL_DEGREE << 2) {
    #pragma clang unroll
    #pragma GCC unroll 8
    for (int i = 0; i < UNROLL_DEGREE; ++i) {
      syms[i] = in[i];
    }

    #pragma clang unroll
    #pragma GCC unroll 8
    for (int i = 0; i < UNROLL_DEGREE; ++i) {
      syms[i] = 0x0000785c | (table[syms[i]] << 16);
    }

    #pragma clang unroll
    #pragma GCC unroll 8
    for (int i = 0; i < UNROLL_DEGREE; ++i) {
      *(uint32_t *) &out[i << 2] = syms[i];
    }
  }

  for (end = end + (n & (UNROLL_DEGREE - 1));in < end; out += 4, ++in) {
    *(uint32_t *) out = 0x0000785c | (table[*in] << 16);
  }

  *out = '\0';
}

void hex_dump_v4(char *restrict out, unsigned char *restrict in, size_t n) {
  unsigned char *restrict end = in + (n & ~7);

  __m128i high, low, cond;
  __m128i highest_digits = _mm_set1_epi8(9);
  __m128i mask = _mm_set1_epi8(15);
  __m128i zero_code = _mm_set1_epi8('0');
  __m128i ten_code = _mm_set1_epi8(0x37);
  __m128i prefix = _mm_set1_epi16(0x785c);

  for (; in < end; in += 8, out += 32) {
    low = _mm_loadu_si128((__m128i *) in);
    high = _mm_srli_epi64(low, 4);

    low = _mm_and_si128(low, mask);
    high = _mm_and_si128(high, mask);

    low = _mm_blendv_epi8(
      _mm_add_epi8(low, zero_code),
      _mm_add_epi8(low, ten_code),
      _mm_cmpgt_epi8(low, highest_digits)
    );

    high = _mm_blendv_epi8(
      _mm_add_epi8(high, zero_code),
      _mm_add_epi8(high, ten_code),
      _mm_cmpgt_epi8(high, highest_digits)
    );

    _mm_store_si128(
      (__m128i *) out,
      _mm_unpacklo_epi16(
        prefix,
        _mm_unpacklo_epi8(high, low)
      )
    );
  }

  for (end = end + (n & 7); in < end; ++in, out += 4) {
    uint32_t next_word = 0x785c;
    uint32_t quotient = *in >> 4;
    uint32_t rem = *in & 15;
    next_word |= (rem > 9 ? rem + 87 : rem + 48) << 24;
    next_word |= (quotient > 9 ? quotient + 87 : quotient + 48) << 16;
    *(uint32_t *) out = next_word;
  }
}

void init_table() {
  for (int i = 0; i < sizeof(table) / sizeof(table[0]); ++i) {
    table[i] = (encoding[i & 15] << 8) | encoding[i >> 4];
  }
}

void randomfill_m5(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c) {
#define CHUNK_SIZE 32
  uint64_t as[CHUNK_SIZE];
  uint64_t cs[CHUNK_SIZE];
  uint64_t *end = out + (n & ~(CHUNK_SIZE - 1));

  as[0] = a;
  cs[0] = c;

  for (int i = 1; i < CHUNK_SIZE; ++i) {
      as[i] = as[i - 1] * as[i - 1];
      cs[i] = cs[i - 1] + as[i - 1] * c;
  }

	while (out < end) {
    #pragma GCC unroll 32
    #pragma GCC ivdep
    for (int i = 0; i < CHUNK_SIZE; ++i) {
      out[i] = x * as[i] + cs[i];
    }

    x = out[CHUNK_SIZE - 1];
    out += CHUNK_SIZE;
	}

  for (int i = n & (CHUNK_SIZE - 1); i; --i) {
    x = x * a + c;
    *end++ = x;
  }
#undef CHUNK_SIZE
}

int main(int argc, char *argv[]) {
  init_table();
  clock_t start, end;
  size_t n = 1 << 17;

  randomfill_m5((uint64_t *) source, n, 1235123, 312351, 1293191);

  for (int i = 0; i < sizeof(test_suite) / sizeof(test_case); ++i) {
    clock_t sum = 0;
    for (int j = 0; j < 1000; ++j) {
      start = clock();
      test_suite[i].func(result, source, n);
      end = clock();
      sum += end - start;
    }

    printf("%s time: %lf\n", test_suite[i].name, (double) (sum) / 1000.0 / (double) CLOCKS_PER_SEC);
    // printf("%s result: %s\n", test_suite[i].name, result);
  }
  
  return 0;
}
