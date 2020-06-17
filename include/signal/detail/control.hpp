#pragma once
#include "emitter.hpp"
#include "receiver.hpp"

namespace signal::detail
{
    template <typename _signal>
    class control 
    {
    public:
        static void connect(emitter_instance<_signal>* emitter, receiver_instance<_signal>* receiver) {
            //If receiver is already connected, close its current connection
            //then make a new connection between emitter and receiver

            if (receiver->is_connected())
                control<_signal>::disconnect(receiver);

            emitter->__connect(receiver);
            receiver->__connect(emitter);
        }
        template <typename F> requires valid_signature<_signal, F>
        static void connect(emitter_instance<_signal>* emitter, receiver_instance<_signal>* receiver, F&& slot) {
            //Connect then assign receiver slot
            control<_signal>::connect(emitter, receiver);
            receiver->set_slot(std::move(slot));
        }

        static void disconnect(detail::emitter_instance<_signal>* emitter) {
            //Disconnect emitter from all receivers
            const auto& receivers = emitter->__receivers();
            for (auto r_ptr : receivers) { r_ptr->__disconnect(); }
            emitter->__disconnect();
        }
        static void disconnect(detail::receiver_instance<_signal>* receiver) {
            //If receiver has a connection, disconnect
            if(receiver->is_connected()) {
                receiver->__emitter()->__disconnect(receiver);
                receiver->__disconnect();
            }
        }
        static void disconnect(detail::emitter_instance<_signal>* emitter, detail::receiver_instance<_signal>* receiver) {
            //If emitter and receiver are connected, disconnect
            if (emitter->__disconnect(receiver))
                receiver->__disconnect();
        }

        static void emit(detail::emitter_instance<_signal>* emitter, typename _signal::tuple_type signal_data) {
            //Distribute signal data through emitter's receivers
            for (auto r_ptr : emitter->__receivers()) {
                r_ptr->__push(signal_data);
            }
        }
        static void receive(receiver_instance<_signal>* receiver) {
            //If slotted feed it data, otherwise flush queue
            if(!receiver->is_slotted()) { receiver->flush(); }
            else {
                while(!receiver->__queue_empty()) {
                    std::apply(receiver->__slot(), receiver->__queue_front());
                    receiver->__pop_queue();
                }
            }
        }
    };
}