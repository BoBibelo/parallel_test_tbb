#include <iostream>
#include <tbb/tbb.h>

int main() {
  tbb::task_scheduler_init init;
  std::cout << "Hello" << std::endl;

  return 0;
}
