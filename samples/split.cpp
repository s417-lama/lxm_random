/*
 * How to run:
 * $ cd samples/
 * $ g++ -std=c++17 -I../include split.cpp -lpthread
 * $ ./a.out 42 10000 4
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <random>
#include <string>
#include <cstdlib>

#include "lxm_random/lxm_random.hpp"

void show_help_and_exit(int argc [[maybe_unused]], char** argv) {
  std::cerr << "Usage: " << argv[0] << " <seed> <n> <n_threads>" << std::endl;
  exit(1);
}

int main(int argc, char** argv) {
  if (argc < 4) {
    show_help_and_exit(argc, argv);
  }

  uint64_t seed = std::atoll(argv[1]);
  long n = std::atol(argv[2]);
  int n_threads = std::atoi(argv[3]);

  lxm_random::lxm_random rng(seed);

  std::vector<std::thread> threads;
  std::mutex mutex;

  for (int i = 0; i < n_threads; i++) {
    threads.emplace_back([=, &mutex, rng = rng.split()]() mutable {
      std::normal_distribution<> normal_dist(0, 2);

      std::map<int, int> hist;
      for (int i = 0; i != n; i++)
        ++hist[std::round(normal_dist(rng))];

      std::unique_lock<std::mutex> lock(mutex);
      std::cout << "Thread " << i << ":" << std::endl;

      for (auto [x, y] : hist)
        std::cout << std::setw(2) << x << ' ' << std::string(y * 80 / n, '*') << std::endl;
    });
  }

  for (auto&& th : threads) {
    th.join();
  }
}
