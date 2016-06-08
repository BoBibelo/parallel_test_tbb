#include <iostream>
#include <random>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
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

void highest_prime_divisors(const unsigned long data[],
                            unsigned long results[], size_t begin,
                            size_t end) {
  for (size_t i = begin; i != end; ++i)
    results[i] = highest_prime_divisor(data[i]);
}


struct apply_hpd {
  void operator() (const tbb::blocked_range<size_t>& r) const {
    highest_prime_divisors(data, results, r.begin(), r.end());
  }

  apply_hpd(const unsigned long d[], unsigned long r[])
    : data(d), results(r)
  {}

  const unsigned long * const data;
  unsigned long * const results;
};

void parallel_hpd(const unsigned long data[], unsigned long results[],
                   size_t begin, size_t end)
{
  tbb::parallel_for(tbb::blocked_range<size_t>(begin, end),
                    apply_hpd(data, results));
}

void test_parallel(unsigned long data[], size_t len) {
  unsigned long *results;
  results = new unsigned long[len];

  double time;
  {scoped_timer t(time);
    parallel_hpd(data, results, 0, len);
  }

  std::cout << "parallel time:  \t" << time << std::endl;
  delete[] results;
}

void test_sequential(unsigned long data[], size_t len) {
  unsigned long *results;
  results = new unsigned long[len];

  double time;
  {scoped_timer t(time);
    highest_prime_divisors(data, results, 0, len);
  }

  std::cout << "sequential time: \t" << time << std::endl;
  delete[] results;
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
  tbb::task_scheduler_init init; // extra parameter: number of workers

  // data length, default to 2^10
  size_t data_len = 1 << 10;
  if (ac > 1)
    data_len = std::strtoull(av[1], nullptr, 10);

  unsigned long *data;
  data = new unsigned long[data_len];
  init_data(data, data_len);

  std::cout << "data length: \t\t" << data_len << std::endl;

  test_sequential(data, data_len);

  test_parallel(data, data_len);

  delete[] data;

  return 0;
}
