#pragma once
#include "emitter.hpp"
#include "receiver.hpp"

namespace signal 
{
    template <typename tag, typename ... signature>
    struct id {
        using __traits = detail::signal_traits<signature ...>;
    };

    #ifndef declare_signal
        /*!
        @brief Declares an unique signal id for a given argument signature
        @def   declare_signal
        @since version 1.0.0
        **/
        #define declare_signal(NAME, ...) \
        struct ___secret_tag_for_##NAME {}; \
        using NAME = signal::id<___secret_tag_for_##NAME __VA_OPT__(,) __VA_ARGS__>
    #endif
}