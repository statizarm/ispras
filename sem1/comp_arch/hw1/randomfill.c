#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

typedef void (*randomfill_func)(uint64_t *, size_t, uint64_t, uint64_t, uint64_t);

struct test_case {
  randomfill_func foo; 
  char *name;
};


void randomfill_o(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c);
void randomfill_m1(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c);
void randomfill_m2(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c);
void randomfill_m3(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c);
void randomfill_m4(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c);
void randomfill_m5(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c);

struct test_case test_suite[] = {
    {
        .foo = randomfill_o,
        .name = "old"
    },
    {
        .foo = randomfill_m1,
        .name = "first modified"
    },
    {
        .foo = randomfill_m2,
        .name = "second modified"
    },
    {
        .foo = randomfill_m3,
        .name = "third modified"
    },
    {
        .foo = randomfill_m4,
        .name = "fourth modified"
    },
    {
        .foo = randomfill_m5,
        .name = "fourth modified"
    }

};

uint64_t dst[999999];

void randomfill_o(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c) {
    for (; n; n--) {
        x = x * a + c; // через итерации x зависит сам от себя true dependency, так как a и c не меняеются, они не влияют на ilp
        *out++ = x; // запись по адресу зависит от x, true dependency
    }
}

/*
** for (;;) {
*1    if (!n) break;
*
*2    x = x * a + c;
*3    *out = x;
*4    ++out;
*5    --n;
** }
*/

void randomfill_m1(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c) {
    uint64_t xa = x * a;

    x = xa + c;
    for (; n; --n) {
        xa = x * a;
        *out = x;
        ++out;
        x = xa + c;
    }
}

void randomfill_m2(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c) {
    uint64_t xa = x * a;

    x = xa + c;
    for (; n; --n) {
        xa = x * a;
        *out = x;
        x = xa + c;
        ++out;
    }
}

void randomfill_m3(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c) {
    uint64_t xa = x * a;
    uint64_t next_x;
    uint64_t next_xa;

    x = xa + c;
    next_xa = x * a;
    next_x = next_xa + c;
    for (; n; --n) {
        *out++ = x;
        x = next_x;
        next_xa = x * a;
        next_x = next_xa + c;
    }
}

void randomfill_m4(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c) {
  uint64_t as[4];
  uint64_t cs[4];
  uint64_t *end = out + (n & ~0x3);

  as[0] = a;
  cs[0] = c;

  for (int i = 1; i < 4; ++i) {
      as[i] = as[i - 1] * as[i - 1];
      cs[i] = cs[i - 1] + as[i - 1] * c;
  }

	while (out < end) {
    out[0] = x * as[0] + cs[0];
    out[1] = x * as[1] + cs[1];
    out[2] = x * as[2] + cs[2];
    out[3] = x * as[3] + cs[3];

    x = out[3];
    out += 4;
	}

  for (int i = n & 0x3; i; --i) {
    x = x * a + c;
    *end++ = x;
  }
}

void randomfill_m5(uint64_t *out, size_t n, uint64_t x, uint64_t a, uint64_t c) {
  const uint64_t chunk_size = 32; // must be power of two
  uint64_t as[chunk_size];
  uint64_t cs[chunk_size];
  uint64_t *end = out + (n & ~(chunk_size - 1));

  as[0] = a;
  cs[0] = c;

  for (int i = 1; i < chunk_size; ++i) {
      as[i] = as[i - 1] * as[i - 1];
      cs[i] = cs[i - 1] + as[i - 1] * c;
  }

	while (out < end) {
    #pragma GCC unroll chunk_size
    #pragma GCC ivdep
    for (int i = 0; i < chunk_size; ++i) {
      out[i] = x * as[i] + cs[i];
    }

    x = out[chunk_size - 1];
    out += chunk_size;
	}

  for (int i = n & (chunk_size - 1); i; --i) {
    x = x * a + c;
    *end++ = x;
  }
}

int main() {
    clock_t start;
    clock_t end;

    for (int i = 0; i < sizeof(test_suite) / sizeof(struct test_case); ++i) {
        memset(dst, 0, sizeof(dst));
        clock_t sum = 0;
        for (int j = 0; j < 100; ++j) {
          start = clock();
          test_suite[i].foo(dst, sizeof(dst) / sizeof(uint64_t), 131, 12346123, 2331345);
          end = clock();

          sum += end - start;
        }

        sum /= 100;

        printf("%s randomfill time: %lf\n", test_suite[i].name, (double) (sum) / (double) CLOCKS_PER_SEC);
    }

    return 0;
}

