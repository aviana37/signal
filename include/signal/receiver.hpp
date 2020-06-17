#pragma once
#include "detail/control.hpp"

namespace signal 
{
    template <typename ... signals>
    class receiver : detail::not_copyable
    {
    private:
        template <typename _signal> using instance_t   = detail::make_receiver_instance<_signal>::type;
        template <typename _signal> using instance_ptr = detail::make_receiver_instance_ptr<_signal>::type;
        template <typename _signal> using emitter_access = detail::emitter_access<_signal>;
        using instances = detail::transform<detail::mixin<signals ...>, detail::make_receiver_instance>;
        instances m_instances;

        template <typename _signal> 
        constexpr auto  get_instance_ptr() { return static_cast<instance_ptr<_signal>>(&m_instances); }
        template <typename _signal>
        constexpr auto& get_instance_ref() { return static_cast<instance_t<_signal>&>(m_instances); }

    public:
        receiver() : m_instances{} {}
        ~receiver() = default;

        template <typename _signal> 
        operator detail::receiver_access<_signal>() { return { get_instance_ptr<_signal>() }; }

        template <typename _signal>
        void connect(emitter_access<_signal> emitter) {
            detail::control<_signal>::connect(emitter.instance, get_instance_ptr<_signal>());
        }
        template <typename _signal, typename F> requires detail::valid_signature<_signal, F>
        void connect(emitter_access<_signal> emitter, F&& slot) {
            detail::control<_signal>::connect(emitter.instance, get_instance_ptr<_signal>(), std::move(slot));
        }
        template <typename _signal, typename T, typename F>
        void connect(emitter_access<_signal> emitter, F&& member_function, T* class_instance) {
            connect(emitter, _signal::make_slot(std::move(member_function), class_instance));
        }

        template <typename _signal>
        void disconnect(emitter_access<_signal> emitter) {
            detail::control<_signal>::disconnect(emitter.instance, get_instance_ptr<_signal>());
        }
        template <typename _signal>
        void disconnect() {
            detail::control<_signal>::disconnect(get_instance_ptr<_signal>());
        }

        template <typename _signal>
        void receive() {
            detail::control<_signal>::receive(get_instance_ptr<_signal>());
        }

        template <typename _signal> void block()    { get_instance_ref<_signal>().block();   }
        template <typename _signal> void unblock()  { get_instance_ref<_signal>().unblock(); }
        template <typename _signal> void flush()    { get_instance_ref<_signal>().flush();   }

        template <typename _signal, typename F> requires detail::valid_signature<_signal, F>
        void set_slot(F&& f) { get_instance_ref<_signal>().set_slot(std::move(f)); }

        template <typename _signal>
        bool is_connected(emitter_access<_signal> emitter) {
            return get_instance_ref<_signal>().is_connected(emitter.instance);
        }
        template <typename _signal>
        bool is_connected() {
            return get_instance_ref<_signal>().is_connected();
        }
        template <typename _signal>
        bool is_blocked() {
            return get_instance_ref<_signal>().is_blocked();
        }
        template <typename _signal>
        bool is_slotted() {
            return get_instance_ref<_signal>().is_slotted();
        }
    };

}