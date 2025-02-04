#ifndef CPPUTILS_WAIT_FREE_COUNTER_H
#define CPPUTILS_WAIT_FREE_COUNTER_H

#include <atomic>

/*
class: Counter
description: Implementation of a wait-free counter for tracking
when the counter  hits zero between many threads. Transcribed directly
from Daniel Anderson's CppCon 2024 Talk 'Introduction to Wait-free Algorithms'
youtube link: https://youtu.be/kPh8pod0-gk?si=zFQRgrVCA8OsBja0
*/

class Counter
{
    static constexpr uint64_t is_zero = 1ull << 63;
    static constexpr uint64_t helped = 1ull << 62;

public:
    bool increment_if_not_zero() { return (counter.fetch_add(1) & is_zero) == 0; }

    bool decrement()
    {
        if (counter.fetch_sub(1) == 1) {
            uint64_t e = 0;
            if (counter.compare_exchange_strong(e, is_zero))
                return true;
            else if ((e & helped) && (counter.exchange(is_zero) & helped))
                return true;
        }
        return false;
    }

    uint64_t read()
    {
        auto val = counter.load();
        if (val == 0 && counter.compare_exchange_strong(val, is_zero | helped)) return 0;
        return (val & is_zero) ? 0 : val;
    }

private:
    std::atomic<uint64_t> counter{1};
};

#endif