# LXM Random Number Generator in C++

This C++17 header-only library `lxm_random` is an (unofficial) implementation of the LXM random number generator, which was presented in the following paper:

[Guy L. Steele Jr. and Sebastiano Vigna. "LXM: better splittable pseudorandom number generators (and almost as fast)" in ACM OOPSLA '21.](https://doi.org/10.1145/3485525)

LXM is **splittable**, which means that a random number generator (RNG) can be splitted into two (apparently) independent streams of RNGs.
This split operation is particularly useful for parallel computing; an RNG can be split into multiple child RNGs to be passed to child threads, even recursively.
Therefore, the threads can independently generate random numbers in a deterministic manner, without interacting with other threads.

## Usage

The C++ header `lxm_random/lxm_random.hpp` is included in the `include/` directory.

Minimal example:
```cpp
#include <iostream>
#include "lxm_random/lxm_random.hpp"

int main() {
  // Initialize an RNG with a random seed
  uint64_t seed = 42;
  lxm_random::lxm_random rng(seed);

  // Generate a random number
  std::cout << rng() << std::endl;
}
```

See [./samples/demo.cpp](./samples/demo.cpp) for more practical usage.

## Split Operation

How to split random number generators and pass them to child threads:
```cpp
// Create a root random number generator with a seed
lxm_random::lxm_random rng(seed);

std::vector<std::thread> threads;
for (int i = 0; i < n_threads; i++) {
  // Fork a new thread and pass a new random number generator to it
  // by splitting the root random number generator
  threads.emplace_back([=, rng = rng.split()]() mutable {
    ...
    rng(); // Generate a random number independently from other threads
    ...
  });
}
```

Alternative approach:
```cpp
// Create a root random number generator with a seed
lxm_random::lxm_random rng(seed);

std::vector<std::thread> threads;
for (int i = 0; i < n_threads; i++) {
  // Fork a new thread
  threads.emplace_back([=]() mutable {
    // Split the copied random number generator by giving a unique thread ID
    // so that threads with different IDs will have independent streams
    rng = rng.split(i);
    ...
    rng(); // Generate a random number independently from other threads
    ...
  });
}
```

## Notes

- Currently, only `L64X128MixRandom` variant is implemented
