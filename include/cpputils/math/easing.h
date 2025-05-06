#ifndef CPPUTILS_MATH_EASINGS_H
#define CPPUTILS_MATH_EASINGS_H

#include <functional>

#include "mathConcepts.h"

namespace cpputils {
namespace math {
    template <FloatingPoint T>
    using EasingFunction = T(*)(const T);

    template <FloatingPoint T>
    static T linearInterpolate(const T elapsedRatio)
    {
        return elapsedRatio;
    }

    template <FloatingPoint T>
    static T easeInOutQuad(const T elapsedRatio)
    {
        const T& x = elapsedRatio;
        return x < 0.5 ? 2 * x * x : 1 - ((-2 * x + 2)*(-2 * x + 2)) / 2;
    }

    enum class EasingFuncType {
        LINEAR_INTERPOLATE,
        EASE_IN_OUT_QUAD
    };

    template <FloatingPoint T>
    EasingFunction<T> getEasingFunction(const EasingFuncType funcType) {
        switch (funcType) {
            case EasingFuncType::LINEAR_INTERPOLATE: {
                return linearInterpolate;
                break;
            }
            case EasingFuncType::EASE_IN_OUT_QUAD: {
                return easeInOutQuad;
                break;
            }
        }
        return linearInterpolate;
    }
}
}

#endif
