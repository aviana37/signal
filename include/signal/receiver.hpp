#pragma once
#include "detail/control.hpp"

namespace signal 
{
    template <typename ... signals>
    class receiver final : detail::not_copyable
    {
    private:
        template <typename> friend class detail::control;
        template <typename Signal> using instance_ptr = detail::make_receiver_instance_ptr<Signal>::type;
        using instances = detail::transform<detail::mixin<signals ...>, detail::make_receiver_instance>;
        instances m_instances;

        template <typename Signal> 
        constexpr auto get_instance_ptr() { 
            return static_cast<instance_ptr<Signal>>(&m_instances);
        }

    public:
        receiver() : m_instances{} {}
        ~receiver() = default;

        template <typename Signal, typename E>
        void connect(E& emitter) {
            detail::control<Signal>::connect(detail::control<Signal>::instance_access(emitter), 
                get_instance_ptr<Signal>());
        }
        template <typename Signal, typename E, typename F> requires detail::valid_signature<Signal, F>
        void connect(E& emitter, F&& slot) {
            detail::control<Signal>::connect(detail::control<Signal>::instance_access(emitter), 
                get_instance_ptr<Signal>(), std::move(slot));
        }
        template <typename Signal, typename E, typename T, typename F>
        void connect(E& emitter, F&& member_function, T* class_instance) {
            connect<Signal>(emitter, Signal::make_slot(std::move(member_function), class_instance));
        }

        template <typename Signal, typename E>
        void disconnect(E& emitter) {
            detail::control<Signal>::disconnect(detail::control<Signal>::instance_access(emitter), 
                get_instance_ptr<Signal>());
        }
        template <typename Signal>
        void disconnect() {
            detail::control<Signal>::disconnect(get_instance_ptr<Signal>());
        }

        template <typename Signal>
        void receive() {
            detail::control<Signal>::receive(get_instance_ptr<Signal>());
        }

        template <typename Signal> void block()    { get_instance_ptr<Signal>()->block();   }
        template <typename Signal> void unblock()  { get_instance_ptr<Signal>()->unblock(); }
        template <typename Signal> void flush()    { get_instance_ptr<Signal>()->flush();   }

        template <typename Signal, typename F> requires detail::valid_signature<Signal, F>
        void set_slot(F&& f) { get_instance_ptr<Signal>()->set_slot(std::move(f)); }

        template <typename Signal, typename E>
        bool is_connected(E& emitter) {
            return get_instance_ptr<Signal>()->
                is_connected(detail::control<Signal>::instance_access(emitter));
        }
        template <typename Signal>
        bool is_connected() {
            return get_instance_ptr<Signal>()->is_connected();
        }
        template <typename Signal>
        bool is_blocked() {
            return get_instance_ptr<Signal>()->is_blocked();
        }
        template <typename Signal>
        bool is_slotted() {
            return get_instance_ptr<Signal>()->is_slotted();
        }
    };

}