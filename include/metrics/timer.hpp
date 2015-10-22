#pragma once

#include <future>

#include "metrics/metric.hpp"

namespace metrics {

/// \type `Accumulate` must meet `Accumulate` requirements.
template<class Accumulate>
class timer : public metric_t {
public:
    typedef std::chrono::high_resolution_clock clock_type;
    typedef clock_type::time_point time_point;

    typedef Accumulate accumulator_type;

    class context_t {
        timer* parent;
        const time_point timestamp;

    public:
        explicit
        context_t(timer* parent);
        context_t(const context_t& other) = delete;
        context_t(context_t&&) = default;

        ~context_t();

        context_t&
        operator=(const context_t& other) = delete;

        context_t&
        operator=(context_t&& other) = delete;
    };

    // friend class context_t;
    typedef context_t context_type;

public:
    timer(std::string name, processor_t& processor);

    std::uint64_t
    count() const;

    // TODO: All `meter` getters.
    // TODO: All `snapshot` getters.

    template<typename F>
    auto
    measure(F fn) -> decltype(fn()) {
        context_t context(this);
        return fn();
    }

    context_type
    context();
};

}  // namespace metrics