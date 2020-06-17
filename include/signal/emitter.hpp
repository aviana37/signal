#pragma once
#include "detail/control.hpp"

namespace signal 
{
    template <typename ... signals>
    class emitter : detail::not_copyable
    {
    private:
        template <typename _signal> using instance_t   = detail::make_emitter_instance<_signal>::type;
        template <typename _signal> using instance_ptr = detail::make_emitter_instance_ptr<_signal>::type;
        template <typename _signal> using receiver_access = detail::receiver_access<_signal>;
        using instances = detail::transform<detail::mixin<signals ...>, detail::make_emitter_instance>;
        instances m_instances;

        template <typename _signal> 
        constexpr auto  get_instance_ptr() { return static_cast<instance_ptr<_signal>>(&m_instances); }
        template <typename _signal>
        constexpr auto& get_instance_ref() { return static_cast<instance_t<_signal>&>(m_instances); }

    public:
        emitter() : m_instances{} {}
        ~emitter() = default;

        template <typename _signal> 
        operator detail::emitter_access<_signal>() { return { get_instance_ptr<_signal>() }; }

        template <typename _signal>
        void connect(receiver_access<_signal> receiver) {
            detail::control<_signal>::connect(get_instance_ptr<_signal>(), receiver.instance);
        }
        template <typename _signal, typename F> requires detail::valid_signature<_signal, F>
        void connect(detail::receiver_access<_signal> receiver, F&& slot) {
            detail::control<_signal>::connect(get_instance_ptr<_signal>(), receiver.instance, std::move(slot));
        }
        template <typename _signal, typename T, typename F>
        void connect(receiver_access<_signal> receiver, F&& member_function, T* class_instance) {
            connect(receiver, _signal::make_slot(std::move(member_function), class_instance));
        }

        template <typename _signal>
        void disconnect(receiver_access<_signal> receiver) {
            detail::control<_signal>::disconnect(get_instance_ptr<_signal>(), receiver.instance);
        }
        template <typename _signal>
        void disconnect() {
            detail::control<_signal>::disconnect(get_instance_ptr<_signal>());
        }

        template <typename _signal>
        void emit(typename _signal::tuple_type signal_data) {
            detail::control<_signal>::emit(get_instance_ptr<_signal>(), signal_data);
        }

        template <typename _signal>
        bool is_connected(receiver_access<_signal> receiver) {
            return get_instance_ref<_signal>().is_connected(receiver.instance);
        }
    };

}