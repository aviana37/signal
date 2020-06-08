#pragma once
#include "common.hpp"
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

    /*!
    @brief   Emits signals
    @details Alias template for a mixin class that inherits emitter instances for each signal type
    @typedef emitter
    @since   version 1.0.0
    */
    template <typename ... signals>
    using emitter = detail::transform<detail::signal_mixin<signals ...>, detail::make_emitter_instance>;
    
    /*!
    @brief   Receives signals
    @details Alias template for a mixin class that inherits receiver instances for each signal type
    @typedef receiver
    @since   version 1.0.0
    */
    template <typename ... signals>
    using receiver = detail::transform<detail::signal_mixin<signals ...>, detail::make_receiver_instance>;

    /*!
    @brief   Assigns an slot to the receiver and connects it to the emitter
    @details If the receiver is already connected to an emitter, it'll first be disconnected and then proceed to connect to the new emitter
    @fn      void connect(detail::emitter_instance<_signal>& emitter, detail::receiver_instance<_signal>& receiver, F&& slot)
    @param   slot   Callable with signature matching the signal. Member functions must be wrapped in _signal::make_slot(&FooType::member_fun, &foo_instance). 
    */
    template <typename _signal, typename F>
    requires detail::valid_signature<_signal, F>
        void connect(detail::emitter_instance<_signal>& emitter, detail::receiver_instance<_signal>& receiver, F&& slot) {
        if (receiver_access<_signal>::is_connected(&receiver))
            disconnect<_signal>(receiver);

        detail::emitter_access <_signal>::connect(&emitter, &receiver);
        detail::receiver_access<_signal>::connect(&emitter, &receiver, std::move(slot));
    }

    /*!
    @brief  Disconnects the emitter from all connected receivers
    @fn     void disconnect(detail::emitter_instance<_signal>& emitter)
    */
    template <typename _signal>
    void disconnect(detail::emitter_instance<_signal>& emitter) {
        //Disconnect emitter receivers from emitter, if any
        //Then disconnect emitter from receivers
        const auto& receivers = detail::emitter_access<_signal>::receivers(&emitter);
        for (auto r_ptr : receivers) { detail::receiver_access<_signal>::disconnect(r_ptr); }
        detail::emitter_access<_signal>::disconnect(&emitter);
    }
    
    /*!
    @brief  Disconnects the receiver from its emitter
    @fn     void disconnect(detail::receiver_instance<_signal>& receiver)
    */
    template <typename _signal>
    void disconnect(detail::receiver_instance<_signal>& receiver) {
        //If receiver is connected, disconnect
        if(receiver_access<_signal>::is_connected(&receiver)) {
            detail::emitter_access<_signal>::disconnect(*detail::receiver_access<_signal>::emitter(&receiver), &receiver);
            detail::receiver_access<_signal>::disconnect(&receiver);
        }
    }

    /*!
    @brief  Disconnects the receiver from the emitter
    @fn     void disconnect(detail::emitter_instance<_signal>& emitter, detail::receiver_instance<_signal>& receiver)
    */
    template <typename _signal>
    void disconnect(detail::emitter_instance<_signal>& emitter, detail::receiver_instance<_signal>& receiver) {
        //If emitter and receiver are connected, disconnect
        if (detail::emitter_access<_signal>::disconnect(&emitter, &receiver))
            detail::receiver_access<_signal>::disconnect(&receiver);
    }

    /*!
    @brief  Emits signal data
    @fn     void emit(detail::emitter_instance<_signal>& emitter, typename _signal::tuple_type signal_data) 
    */
    template <typename _signal>
    void emit(detail::emitter_instance<_signal>& emitter, typename _signal::tuple_type signal_data) {
        const auto& receivers = detail::emitter_access<_signal>::receivers(&emitter);
        for (auto r_ptr : receivers) { detail::receiver_access<_signal>::push(r_ptr, signal_data); }
    }

    /*!
    @brief  Receives signal data
    @fn     void receive(detail::receiver_instance<_signal>& receiver) 
    */
    template <typename _signal>
    void receive(detail::receiver_instance<_signal>& receiver) {
        detail::receiver_access<_signal>::receive(&receiver);
    }
}