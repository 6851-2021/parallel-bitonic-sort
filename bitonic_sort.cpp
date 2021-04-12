#include <algorithm>
#include <vector>
#include <iostream>
#include <type_traits>
#include <cassert>
#include <cstdlib>

#ifdef NPARALLEL
  #define cilk_for for
  #define cilk_spawn
  #define cilk_sync

  #define bitonic_sort serial_bitonic_sort
#else
  #include <cilk/cilk.h>

  #define bitonic_sort parallel_bitonic_sort
#endif

template <class RandomIt, class Compare>
void serial_bitonic_sort(RandomIt first, RandomIt last, Compare comp) {
  const auto len = std::distance(first, last);
  if (len <= 1) {
    return;
  }
  const auto half_len = len / 2;

  const auto mid = first + half_len;

  using difference_type = typename std::iterator_traits<RandomIt>::difference_type;
  for (difference_type i{}; i < half_len; ++i) {
    const auto left = first + i;
    const auto right = mid + i;
    if (comp(*right, *left)) {
      std::iter_swap(left, right);
    }
  }

  serial_bitonic_sort(first, mid, comp);
  serial_bitonic_sort(mid, last, comp);
}

template <class RandomIt, class Compare>
void parallel_bitonic_sort(RandomIt first, RandomIt last, Compare comp) {
  const auto len = std::distance(first, last);
  if (len <= 4194304) {
    return serial_bitonic_sort(first, last, comp);
  }
  const auto half_len = len / 2;

  const auto mid = first + half_len;

  using difference_type = typename std::iterator_traits<RandomIt>::difference_type;
  for (difference_type i{}; i < half_len; ++i) {
    const auto left = first + i;
    const auto right = mid + i;
    if (comp(*right, *left)) {
      std::iter_swap(left, right);
    }
  }

  cilk_spawn parallel_bitonic_sort(first, mid, comp);
  parallel_bitonic_sort(mid, last, comp);

  cilk_sync;
}

template <class RandomIt>
auto bitonic_sort(RandomIt first, RandomIt last) {
  return bitonic_sort(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>{});
}

template <class RandomIt, class Compare>
bool sorted(RandomIt first, RandomIt last,  Compare comp) {
  for (auto it1 = first, it2 = first + 1; it2 != last; ++it1, ++it2){
    if (comp(*it2, *it1)) {
      return false;
    }
  }
  return true;
}

template <class RandomIt>
auto sorted(RandomIt first, RandomIt last) {
  return sorted(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>{});
}

std::vector<int> make_bitonic_sequence(size_t n) {
  std::vector<int> vec(n);

  const auto switch_point = n / 2 - (n > 8 ? n / 10 : 0);

  for (size_t i = 0; i < switch_point; ++i) {
    vec[i] = 2 * i;
  }

  for (size_t i = switch_point; i < n; ++i) {
    vec[i] = 2 * (2 * switch_point - i - 1) - 1;
  }

  return vec;
}

template <typename T>
void print_seq(T a) {
  for (int i : a) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  const int n = (argc > 1) ? atoi(argv[1]) : 256;
  std::vector<int> a = make_bitonic_sequence(n);

  if (a.size() <= 256) {
    print_seq(a);
  }

  // length must be a power of 2
  assert((a.size() > 0) && ((a.size() & (a.size() - 1)) == 0));
  bitonic_sort(a.begin(), a.end());

  if (a.size() <= 256) {
    print_seq(a);
  }

  std::cout << (sorted(a.begin(), a.end()) ? "Sorted!" : "Not Sorted!") << std::endl;
  return 0;
}
