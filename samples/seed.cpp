/*
 * How to run:
 * $ cd samples/
 * $ g++ -std=c++17 -I../include seed.cpp
 * $ ./a.out 42 100
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "lxm_random/lxm_random.hpp"

void show_help_and_exit(int argc [[maybe_unused]], char** argv) {
  std::cerr << "Usage: " << argv[0] << " <seed> <n>" << std::endl;
  exit(1);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    show_help_and_exit(argc, argv);
  }

  uint64_t seed = std::atoll(argv[1]);
  long n = std::atol(argv[2]);

  lxm_random::lxm_random rng(seed);
  for (long i = 0; i < n; i++) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(16)
      << rng() << std::endl;
  }
}
