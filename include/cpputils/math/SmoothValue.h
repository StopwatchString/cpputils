#ifndef CPPUTILS_MATH_SMOOTH_VALUE_H
#define CPPUTILS_MATH_SMOOTH_VALUE_H

#include "easing.h"

namespace cpputils {
namespace math {
    template <Numeric _ValType, FloatingPoint _DurationType = double>
    class SmoothValue {
    public:
        SmoothValue(const _ValType& val) : val(val) {};

        _ValType get() const { return val; }

        bool isCompleted() { return completed; }

        void timestep(const _DurationType& dt)
        {
            if (!completed) {
                elapsed += dt;
                if (elapsed > duration) {
                    completed = true;
                    elapsed = duration;
                    val = endValue;
                }
                else {
                    val = (endValue - startValue) * func(elapsed / duration) + startValue;
                }
            }
        }

        void target(const _ValType& target, const _DurationType& length, const EasingFunction<_DurationType>& easingFunc)
        {
            completed = false;
            func = easingFunc;

            startValue = val;
            endValue = target;

            elapsed = 0.0;
            duration = length;
        }

    private:
        EasingFunction<_DurationType> func{ linearInterpolate };

        _ValType val{};
        _ValType startValue{};
        _ValType endValue{};
        _DurationType duration{ 0.0 };
        _DurationType elapsed{ 0.0 };
        bool completed{ false };
    };
}
}

#endif
