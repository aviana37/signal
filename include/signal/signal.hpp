#pragma once
#include "common.hpp"
#include "emitter.hpp"
#include "receiver.hpp"

namespace signal
{
    #define declare_signal(SIGNAL_NAME, ...) \
    struct __secret_tag_for_##SIGNAL_NAME {}; \
    using SIGNAL_NAME = signal::detail::signal_id<__secret_tag_for_##SIGNAL_NAME __VA_OPT__(,) __VA_ARGS__>

    template <typename _signal, typename F>
    requires detail::valid_signature<_signal, F>
        void connect(detail::emitter_instance <_signal>& emitter, detail::receiver_instance<_signal>& receiver, F&& slot) {
        if (receiver_access<_signal>::is_connected(&receiver))
            disconnect<_signal>(receiver);

        detail::emitter_access <_signal>::connect(&emitter, &receiver);
        detail::receiver_access<_signal>::connect(&emitter, &receiver, std::move(slot));
    }

    template <typename _signal>
    void disconnect(detail::emitter_instance<_signal>& emitter) {
        const auto& receivers = detail::emitter_access<_signal>::receivers(&emitter);
        for (auto r_ptr : receivers) { detail::receiver_access<_signal>::disconnect(r_ptr); }
        detail::emitter_access<_signal>::disconnect(&emitter);
    }
    template <typename _signal>
    void disconnect(detail::receiver_instance<_signal>& receiver) {
        detail::emitter_access<_signal>::disconnect(detail::receiver_access<_signal>::emitter(&receiver), &receiver);
        detail::receiver_access<_signal>::disconnect(&receiver);
    }
    template <typename _signal>
    void disconnect(detail::emitter_instance <_signal>& emitter, detail::receiver_instance<_signal>& receiver) {
        //if emitter and receiver are connected, disconnect
        if (detail::emitter_access<_signal>::disconnect(&emitter, &receiver))
            detail::receiver_access<_signal>::disconnect(&receiver);
    }

    template <typename _signal>
    void emit(detail::emitter_instance<_signal>& emitter, typename _signal::tuple_type signal_data) {
        const auto& receivers = detail::emitter_access<_signal>::receivers(&emitter);
        for (auto r_ptr : receivers) { detail::receiver_access<_signal>::push(r_ptr, signal_data); }
    }

    template <typename _signal>
    void receive(detail::receiver_instance<_signal>& receiver) {
        detail::receiver_access<_signal>::receive(&receiver);
    }
}