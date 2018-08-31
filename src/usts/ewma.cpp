#include "metrics/usts/ewma.hpp"

namespace metrics {
namespace usts {

template<typename Clock>
ewma<Clock>::ewma(duration interval) :
    tau(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count()),
    average()
{
    if (interval == clock_type::duration::zero()) {
        throw std::invalid_argument("interval can not be zero");
    }
}

template<typename Clock>
auto ewma<Clock>::add(double value) -> void {
    add(clock_type::now(), value);
}

template<typename Clock>
auto ewma<Clock>::add(time_point time, double value) -> void {
    if (initialized.test_and_set()) {
        const auto alpha = calculate_alpha(time);
        average = alpha * value + (1 - alpha) * average;
    } else {
        average = value;
        birthstamp = time;
    }

    prev = time;
}

template<typename Clock>
auto ewma<Clock>::get() const -> double {
    return get(clock_type::now());
}

template<typename Clock>
auto ewma<Clock>::get(time_point time) const -> double {
    const auto alpha = calculate_alpha(time);
    return alpha * average;
}

template<typename Clock>
auto ewma<Clock>::warmed_up() const -> bool {
    const auto elapsed = std::chrono::duration_cast<
        std::chrono::nanoseconds
    >(prev - birthstamp).count();

    return elapsed >= 35 * tau;
}

template<typename Clock>
auto ewma<Clock>::calculate_alpha(time_point time) const -> double {
    if (time < prev) {
        throw std::invalid_argument("time argument must monotonically increase");
    }

    const auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(time - prev).count();
    return std::exp(-delta / tau);
}

template class ewma<std::chrono::high_resolution_clock>;
template class ewma<std::chrono::steady_clock>;

}  // namespace usts
}  // namespace metrics
