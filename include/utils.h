#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>

namespace utils {

inline size_t roundUpToAlignment(const size_t value, size_t alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

} // namespace utils

#endif // UTILS_H_
