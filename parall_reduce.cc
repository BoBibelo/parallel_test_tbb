#include <algorithm>
#include <iostream>
#include <random>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/task_scheduler_init.h>

// From previous practical session
#include "timer.hh"

static inline
bool is_prime(unsigned long n) {
  if (n < 2 || n % 2 == 0) return n == 1 || n == 2;
  unsigned long x = 1 + n / 2;
  for (; x > 1 && n % x != 0; x -= 2) {}
  return x == 1;
}

static inline
unsigned long highest_prime_divisor(unsigned long n) {
  if (n <= 2 || is_prime(n)) return n;
  unsigned long x = n / 2;
  for (; x > 1 && !(n % x == 0 && is_prime(x)); --x) {}
  return x;
}

unsigned long max_prime_divisors(const unsigned long data[], size_t begin, size_t end) {
  // FIX ME !
  unsigned long ans = 0;
  for (size_t it = begin; it != end; it++)
  {
    unsigned long tmp = highest_prime_divisor(data[it]);
    ans = ans > tmp ? ans : tmp;
  }
  return ans;
}

void test_sequential(unsigned long data[], size_t len) {
  double time;
  double max_val;
  {scoped_timer t(time);
    max_val = max_prime_divisors(data, 0, len);
  }

  std::cout << "sequential max: \t" << max_val << std::endl;
  std::cout << "          time: \t" << time << std::endl;
}

struct find_max_hpd {
  const unsigned long * const data;
  unsigned long max_val;

  void operator() (const tbb::blocked_range<size_t>& r) {
    max_val = std::max(max_val, max_prime_divisors(data, r.begin(), r.end()));
  }

  void join(const find_max_hpd& x) {
    max_val = std::max(max_val, x.max_val);
  }

  find_max_hpd(find_max_hpd& x, tbb::split)
    : data(x.data), max_val(1) {}

  find_max_hpd(const unsigned long d[])
    : data(d), max_val(1) {}
};

unsigned long parallel_max_hpd(const unsigned long data[], size_t begin,
                               size_t end) {
  find_max_hpd op(data);
  tbb::parallel_reduce(tbb::blocked_range<size_t>(begin, end), op);
  return op.max_val;
}

void test_parallel(const unsigned long data[], size_t len) {
  unsigned long max_val;

  double time;
  {scoped_timer t(time);
    max_val = parallel_max_hpd(data, 0, len);
  }

  std::cout << "parallel   max: \t" << max_val << std::endl;
  std::cout << "          time: \t" << time << std::endl;
}

// random fill of vector
static inline
void init_data(unsigned long data[], size_t len) {
  std::random_device                    rd;
  std::mt19937                          gen(rd());
  std::uniform_int_distribution<>       dis(0, 1 << 16);

  for (size_t i = 0; i != len; ++i)
    data[i] = dis(gen);
}

int main(int ac, char *av[]) {
  tbb::task_scheduler_init      init; // extra parameter: number of workers

  // data length, default to 2^10
  size_t                        data_len = 1 << 10;
  if (ac > 1)
    data_len = std::strtoull(av[1], nullptr, 10);

  unsigned long *data;
  data = new unsigned long[data_len];
  init_data(data, data_len);

  std::cout << "data length: \t\t" << data_len << std::endl;

  test_sequential(data, data_len);

  test_parallel(data, data_len);

  return 0;
}
