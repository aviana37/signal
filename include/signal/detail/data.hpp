#pragma once
#include <mutex>
#include <memory>

namespace signal::detail
{
    template <typename ... types>
    struct unguarded_data {
        std::tuple<std::remove_reference_t<types> ...> tuple;
        template <typename F>
        decltype(auto) apply(F func) {
            return std::apply(func, tuple);
        }
    };
    template <typename ... types>
    struct guarded_data {
        std::tuple<std::remove_reference_t<types> ...> tuple;
        template <typename F>
        decltype(auto) apply(F func) {
            static std::mutex m;
            std::scoped_lock lock(m);
            return std::apply(func, tuple);
        }
    };

    template <typename ... types>
    struct read_only {
        static constexpr bool value {(!std::is_reference_v<types> && ...)};
    };

    template <bool unguarded, typename ... types>
    struct signal_dataT;
    template <typename ... types>
    struct signal_dataT<true, types ...> {
        using type = unguarded_data<types ...>;
    };
    template <typename ... types>
    struct signal_dataT<false, types ...> {
        using type = guarded_data<types ...>;
    };

    template <typename ... types>
    using signal_data = typename signal_dataT<read_only<types ...>::value, types ...>::type;
    template <typename ... types>
    using unguarded_signal_data = typename signal_dataT<true, types ...>::type;

    template <typename Signal>
    using shared_data = std::shared_ptr<Signal::traits::data_type>;
}