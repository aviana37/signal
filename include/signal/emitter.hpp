#pragma once
#include "common.hpp"
#include <forward_list>

namespace signal
{
    namespace detail
    {
        template <typename _signal>
        class emitter_instance
        {
        public:
            emitter_instance() : m_receivers{} {}
            ~emitter_instance() {
                for (auto r : m_receivers) {
                    if (r) signal::disconnect(*r);
                }
            }

        private:
            using receiver_ptr = receiver_instance<_signal>*;
            using list = std::forward_list<receiver_ptr>;
            list m_receivers;

            const auto& receivers() {
                return m_receivers;
            }
            void connect(receiver_ptr receiver) {
                m_receivers.push_front(receiver);
            }
            void disconnect() {
                m_receivers.clear();
            }
            bool disconnect(receiver_ptr receiver) {
                return m_receivers.remove(receiver) > 0;
            }

            friend class emitter_access<_signal>;
        };

        template <typename _signal>
        struct make_emitter_instance {
            using type = emitter_instance<_signal>;
        };

        template <typename _signal>
        struct emitter_access
        {
            static const auto& receivers(emitter_instance<_signal>* e) {
                return e->receivers();
            }
            static void connect(emitter_instance<_signal>* e, receiver_instance<_signal>* r) {
                e->connect(r);
            }
            static void disconnect(emitter_instance<_signal>* e) {
                e->disconnect();
            }
            static bool disconnect(emitter_instance<_signal>* e, receiver_instance<_signal>* r) {
                return e->disconnect(r);
            }
        };
    }
}