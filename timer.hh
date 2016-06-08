#ifndef _PrPa_TP01_TIMER_HH_
#define _PrPa_TP01_TIMER_HH_

#include <chrono>

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::steady_clock;

struct scoped_timer {
  scoped_timer(double& s) : seconds(s), t0(steady_clock::now()) {}

  ~scoped_timer() {
    auto t1 = steady_clock::now();
    seconds = (t1 - t0).count() / 1e9;
  }

  double& seconds;
  steady_clock::time_point t0;
};

#endif
