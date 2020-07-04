#pragma once
#include "emitter.hpp"
#include "receiver.hpp"

namespace signal::detail
{
    template <typename Signal>
    class control 
    {
    public:
        template <typename T>
        static constexpr auto instance_access(T& t) {
            return t.template get_instance_ptr<Signal>();
        }

        static void connect(emitter_instance<Signal>* emitter, receiver_instance<Signal>* receiver) {
            //If receiver is already connected, close its current connection
            //then make a new connection between emitter and receiver

            if (receiver->is_connected())
                control<Signal>::disconnect(receiver);

            emitter->c_connect(receiver);
            receiver->c_connect(emitter);
        }
        template <typename F> requires valid_signature<Signal, F>
        static void connect(emitter_instance<Signal>* emitter, receiver_instance<Signal>* receiver, F&& slot) {
            //Connect then assign receiver slot
            control<Signal>::connect(emitter, receiver);
            receiver->set_slot(std::move(slot));
        }

        static void disconnect(detail::emitter_instance<Signal>* emitter) {
            //Disconnect emitter from all receivers
            const auto& receivers = emitter->c_receivers();
            for (auto r_ptr : receivers) { r_ptr->c_disconnect(); }
            emitter->c_disconnect();
        }
        static void disconnect(detail::receiver_instance<Signal>* receiver) {
            //If receiver has a connection, disconnect
            if(receiver->is_connected()) {
                receiver->c_emitter()->c_disconnect(receiver);
                receiver->c_disconnect();
            }
        }
        static void disconnect(detail::emitter_instance<Signal>* emitter, detail::receiver_instance<Signal>* receiver) {
            //If emitter and receiver are connected, disconnect
            if (emitter->c_disconnect(receiver))
                receiver->c_disconnect();
        }

        static void emit(detail::emitter_instance<Signal>* emitter, auto&& signal_data) {
            //Distribute signal data through emitter's receivers
            for (auto r_ptr : emitter->c_receivers()) {
                r_ptr->c_push(signal_data);
            }
        }
        static void receive(receiver_instance<Signal>* receiver) {
            //If slotted feed it data, otherwise flush queue
            if(!receiver->is_slotted()) { receiver->flush(); }
            else {
                while(!receiver->c_queue_empty()) {
                    std::apply(receiver->c_slot(), receiver->c_queue_front());
                    receiver->c_pop_queue();
                }
            }
        }
    };
}