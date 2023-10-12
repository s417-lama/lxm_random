#pragma once

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace lxm_random {

// The first 64 bits of the golden ratio (1+sqrt(5))/2, forced to be odd.
constexpr inline uint64_t golden_ratio_64 = 0x9e3779b97f4a7c15ull;

// The first 64 bits of the silver ratio 1+sqrt(2), forced to be odd.
constexpr inline uint64_t silver_ratio_64 = 0x6A09E667F3BCC909ull;

// MurmurHash3
// https://github.com/aappleby/smhasher/wiki/MurmurHash3
constexpr inline uint64_t mix_murmur(uint64_t h) {
  h ^= h >> 33;
  h *= 0xff51afd7ed558ccdull;
  h ^= h >> 33;
  h *= 0xc4ceb9fe1a85ec53ull;
  h ^= h >> 33;
  return h;
}

// David Stafford's mixing function
// http://zimbry.blogspot.com/2011/09/better-bit-mixing-improving-on.html
constexpr inline uint64_t mix_stafford13(uint64_t h) {
  h ^= h >> 30;
  h *= 0xbf58476d1ce4e5b9ull;
  h ^= h >> 27;
  h *= 0x94d049bb133111ebull;
  h ^= h >> 31;
  return h;
}

// Doug Lea's mixing function
constexpr inline uint64_t mix_lea(uint64_t h) {
  h ^= h >> 32;
  h *= 0xdaba0b6eb09322e3ull;
  h ^= h >> 32;
  h *= 0xdaba0b6eb09322e3ull;
  h ^= h >> 32;
  return h;
}

// C++20 feature
template <typename T>
constexpr inline T rotl(T x, int s) {
  constexpr int N = std::numeric_limits<T>::digits;
  return (x << s) | (x >> (N - s));
}

class l64x128mix_random {
public:
  using result_type = uint64_t;

  constexpr static result_type default_seed = 1ull;

private:
  template <typename SeedSeq>
  using if_seed_seq = std::enable_if_t<!std::is_convertible_v<SeedSeq, l64x128mix_random> &&
                                       !std::is_convertible_v<SeedSeq, result_type>>;

public:
  l64x128mix_random() : l64x128mix_random(default_seed) {}

  explicit l64x128mix_random(result_type value) {
    seed(value);
  }

  template <typename SeedSeq, typename = if_seed_seq<SeedSeq>>
  explicit l64x128mix_random(SeedSeq& sseq) {
    seed(sseq);
  }

  l64x128mix_random(uint64_t a, uint64_t s, uint64_t x0, uint64_t x1) {
    init(a, s, x0, x1);
  }

  void seed(result_type value) {
    value ^= silver_ratio_64;
    init(mix_murmur(value),
         1,
         mix_stafford13(value),
         mix_stafford13(value + golden_ratio_64));
  }

  template <typename SeedSeq, typename = if_seed_seq<SeedSeq>>
  void seed(SeedSeq& sseq) {
    uint32_t states[8];
    sseq.generate(states, states + 8);
    init(static_cast<uint64_t>(states[0]) << 32 | states[1],
         static_cast<uint64_t>(states[2]) << 32 | states[3],
         static_cast<uint64_t>(states[4]) << 32 | states[5],
         static_cast<uint64_t>(states[6]) << 32 | states[7]);
  }

  result_type operator()() {
    // Mixing function (lea64)
    uint64_t z = mix_lea(s_ + x0_);

    // Update the LCG subgenerator
    s_ = m_ * s_ + a_;

    // Update the XBG subgenerator (xoroshiro128v1_0)
    uint64_t q0 = x0_, q1 = x1_;
    q1 ^= q0;
    q0 = rotl(q0, 24);
    q0 = q0 ^ q1 ^ (q1 << 16);
    q1 = rotl(q1, 37);
    x0_ = q0, x1_ = q1;

    return z;
  }

  void discard(unsigned long long z) {
    for (unsigned long long i = 0; i < z; i++) {
      (*this)();
    }
  }

  constexpr static result_type min() { return 0; }
  constexpr static result_type max() { return std::numeric_limits<result_type>::max(); }

  friend bool operator==(const l64x128mix_random& r1, const l64x128mix_random& r2) {
    return r1.a_  == r2.a_ &&
           r1.s_  == r2.s_ &&
           r1.x0_ == r2.x0_ &&
           r1.x1_ == r2.x1_;
  }

  friend bool operator!=(const l64x128mix_random& r1, const l64x128mix_random& r2) {
    return !(r1 == r2);
  }

  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits>&
  operator<<(std::basic_ostream<CharT, Traits>& ostream, const l64x128mix_random& r) {
    ostream << r.a_ << " " << r.s_ << " " << r.x0_ << " " << r.x1_;
    return ostream;
  }

  template <typename CharT, typename Traits>
  friend std::basic_istream<CharT, Traits>&
  operator>>(std::basic_istream<CharT, Traits>& istream, l64x128mix_random& r) {
    istream >> r.a_ >> r.s_ >> r.x0_ >> r.x1_;
    return istream;
  }

private:
  void init(uint64_t a, uint64_t s, uint64_t x0, uint64_t x1) {
    a_  = a | 1;
    s_  = s;
    x0_ = x0;
    x1_ = x1;

    // Both x0 and x1 cannot be 0
    if (x0_ == 0 && x1_ == 0) {
      uint64_t v = s_ + golden_ratio_64;
      x0_ = mix_stafford13(v);
      x0_ = mix_stafford13(v + golden_ratio_64);
    }
  }

  constexpr static uint64_t m_ = 0xd1342543de82ef95ull;

  uint64_t a_;
  uint64_t s_;
  uint64_t x0_;
  uint64_t x1_;
};

// Default RNG
using lxm_random = l64x128mix_random;

}
