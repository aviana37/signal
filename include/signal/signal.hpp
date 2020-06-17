#pragma once
#include "emitter.hpp"
#include "receiver.hpp"

namespace signal 
{
    /*!
    @brief Macro to define an unique signal type alias for a given argument signature
    @def   declare_signal
    @since version 1.0.0
    @warning This macro declares a struct type in the current scope for tag dispatching
    */
    #define declare_signal(SIGNAL_NAME, ...) \
    struct __secret_tag_for_##SIGNAL_NAME {}; \
    using SIGNAL_NAME = signal::detail::signal_id<__secret_tag_for_##SIGNAL_NAME __VA_OPT__(,) __VA_ARGS__>

}