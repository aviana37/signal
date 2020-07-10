#pragma once
#include <mutex>

namespace signal::detail
{
    //to-do Allow user to share unguarded reference data
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
    struct shared_dataT;
    template <typename ... types>
    struct shared_dataT<true, types ...> {
        using type = unguarded_data<types ...>;
    };
    template <typename ... types>
    struct shared_dataT<false, types ...> {
        using type = guarded_data<types ...>;
    };

    template <typename ... types>
    using shared_data = typename shared_dataT<read_only<types ...>::value, types ...>::type;
}