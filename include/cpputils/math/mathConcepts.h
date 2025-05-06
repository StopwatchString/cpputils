#ifndef CPPUTILS_MATH_MATH_CONCEPTS_H
#define CPPUTILS_MATH_MATH_CONCEPTS_H

#include <concepts>

namespace cpputils {
namespace math {
    template <typename T>
    concept Numeric = std::is_arithmetic_v<T>;

    template <typename T>
    concept FloatingPoint = std::is_floating_point_v<T>;
}
}

#endif
