#pragma once
#include "detail/control.hpp"

namespace signal 
{
    template <typename ... signals>
    class emitter final : detail::not_copyable
    {
    private:
        template <typename> friend class detail::control;
        template <typename Signal> using instance_ptr = detail::make_emitter_instance_ptr<Signal>::type;
        using instances = detail::transform<detail::mixin<signals ...>, detail::make_emitter_instance>;
        instances m_instances;

        template <typename Signal> 
        constexpr auto get_instance_ptr() { 
            return static_cast<instance_ptr<Signal>>(&m_instances); 
        }

    public:
        emitter() : m_instances{} {}
        ~emitter() = default;

        template <typename Signal, typename R>
        void connect(R& receiver) {
            detail::control<Signal>::connect(
                get_instance_ptr<Signal>(), 
                detail::control<Signal>::instance_access(receiver));
        }

        template <typename Signal, typename R>
        void disconnect(R& receiver) {
            detail::control<Signal>::disconnect(
                get_instance_ptr<Signal>(), 
                detail::control<Signal>::instance_access(receiver));
        }
        template <typename Signal>
        void disconnect() {
            detail::control<Signal>::disconnect(get_instance_ptr<Signal>());
        }

        template <typename Signal>
        void emit(typename Signal::traits::tuple_type&& signal_data) {
            detail::control<Signal>::emit(get_instance_ptr<Signal>(), std::move(signal_data));
        }

        template <typename Signal, typename R>
        bool is_connected(R& receiver) {
            return get_instance_ptr<Signal>()->
                is_connected(detail::control<Signal>::instance_access(receiver));
        }
    };

}