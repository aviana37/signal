#pragma once
#include "common.hpp"
#include <queue>


namespace signal 
{ 
    template <typename ... signals> 
    class receiver; 
}

namespace signal::detail
{
    template <typename _signal>
    class receiver_instance : not_copyable
    {
    public:
        receiver_instance() : m_block{false}, m_slot{}, m_queue{}, m_emitter{nullptr} {}
        ~receiver_instance() { if(m_emitter) control<_signal>::disconnect(m_emitter, this); }

    private:
        friend class control<_signal>;
        template <typename> friend class signal::receiver;

        using tuple = typename _signal::__traits::tuple_type;
        using slot  = typename _signal::__traits::slot_type;
        using emitter_ptr   = emitter_instance<_signal>*;
        using signal_queue  = std::queue<tuple>;

        bool m_block; slot m_slot;
        signal_queue m_queue;
        emitter_ptr m_emitter;

        /* control only */
        void __connect(emitter_ptr emitter) {
            m_emitter = emitter;
        }
        void __disconnect() {
            m_emitter = nullptr;
        }
        auto __emitter() {
            return m_emitter;
        }
        void __pop_queue() {
            m_queue.pop();
        }
        void __push(tuple signal_data) {
            if(!m_block) m_queue.push(signal_data);
        }
        bool __queue_empty() {
            return m_queue.empty();
        }
        auto& __queue_front() {
            return m_queue.front();
        }
        auto& __slot() {
            return m_slot;
        }
        /* control only */


        void block()    { m_block = true;  }
        void unblock()  { m_block = false; }
        void flush()    { while(!m_queue.empty()) m_queue.pop(); }

        bool is_blocked()   { return m_block;   }
        bool is_connected() { return m_emitter; }
        bool is_connected(emitter_ptr emitter)  {
            return emitter == m_emitter;
        }
        bool is_slotted()   { return (bool) m_slot; }

        template <typename F> requires valid_signature<_signal, F>
        void set_slot(F&& f) { m_slot = std::move(f); }
    };

    template <typename _signal>
    struct receiver_access {
        receiver_instance<_signal>* instance = nullptr;
    };

    template <typename _signal>
    struct make_receiver_instance     { using type = receiver_instance<_signal>; };
    template <typename _signal>
    struct make_receiver_instance_ptr { using type = receiver_instance<_signal>*; };
}