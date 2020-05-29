#pragma once
#include "common.hpp"
#include "emitter.hpp"
#include <deque>

namespace signal 
{
    namespace detail 
    {
        template <typename _signal>
        class receiver_instance 
        {
        public:
            receiver_instance() : m_block{false}, m_slot{}, m_queue{}, m_emitter{nullptr} {}
            ~receiver_instance() {
                if(m_emitter) emitter_access<_signal>::disconnect(m_emitter, this);
            }

        private:
            using tuple = typename _signal::tuple_type;
            using slot  = typename _signal::slot;
            using emitter_ptr   = emitter_instance<_signal>*;
            using signal_queue  = std::deque<tuple>;

            bool m_block {false};
            slot m_slot {};
            signal_queue m_queue {};
            emitter_ptr m_emitter {nullptr};

            void flush_queue() {
                m_queue.clear();
            }

            template <typename F>
            requires valid_signature<_signal, F>
            void set_slot(F&& f) {
                m_slot = std::move(f);
            }

            void receive() {
                if(is_slotted()) {
                    while(!m_queue.empty()) {
                        std::apply(m_slot, m_queue.front());
                        m_queue.pop_front();
                    }
                }
            }
            void push(tuple&& signal_data) {
                if(!m_block) {
                    m_queue.push_back(std::move(signal_data));
                }
            }

            void connect(emitter_ptr emitter) {
                m_emitter = emitter;
                m_block = false;
            }
            void disconnect() {
                m_emitter = nullptr;
            }
            void block() {
                m_block = true;
            }
            void unblock() {
                m_block = false;
            }

            emitter_ptr emitter() {
                return m_emitter;
            }
            bool is_connected() {
                return m_emitter;
            }
            bool is_blocked() {
                return m_block;
            }
            bool is_slotted() {
                return (bool) m_slot;
            }

            friend class signal::receiver_access<_signal>;
            friend class detail::receiver_access<_signal>;
        };

        template <typename _signal>
        struct receiver_access
        {
            static void receive(detail::receiver_instance<_signal>* r) {
                r->receive();
            }
            static void push(detail::receiver_instance<_signal>* r, detail::receiver_instance<_signal>::tuple signal_data) {
                r->push(std::move(signal_data));
            }
            template<typename F> requires detail::valid_signature<_signal, F>
            static void connect(detail::emitter_instance<_signal>* e, detail::receiver_instance<_signal>* r, F&& f) {
                r->connect(e);
                r->set_slot(std::move(f));
                r->flush_queue();
            }
            static void disconnect(detail::receiver_instance<_signal>* r) {
                r->disconnect();
            }
            static detail::receiver_instance<_signal>::emitter_ptr emitter(detail::receiver_instance<_signal>* r) {
                return r->emitter();
            }
        };

        template <typename _signal>
        struct make_receiver_instance {
            using type = receiver_instance<_signal>;
        };
    }

    template <typename _signal>
    struct receiver_access 
    {
        template<typename F> requires detail::valid_signature<_signal, F>
        static void set_slot(detail::receiver_instance<_signal>* r, F&& f) {
            r->set_slot(std::move(f));
        }

        static void flush_queue(detail::receiver_instance<_signal>* r) {
            r->flush_queue();
        }

        static void block(detail::receiver_instance<_signal>* r) {
            r->block();
        }
        static void unblock(detail::receiver_instance<_signal>* r) {
            r->unblock();
        }

        static bool is_connected(detail::receiver_instance<_signal>* r) {
            return r->is_connected();
        }
        static bool is_blocked(detail::receiver_instance<_signal>* r) {
            return r->is_blocked();
        }
        static bool is_slotted(detail::receiver_instance<_signal>* r) {
             return r->is_slotted();
        }
    };

    template <typename ... signals>
    using receiver = detail::transform<detail::signal_mixin<signals ...>, detail::make_receiver_instance>;
}