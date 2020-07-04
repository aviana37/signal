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
    template <typename Signal>
    class receiver_instance : not_copyable
    {
    public:
        receiver_instance() : m_block{false}, m_slot{}, m_queue{}, m_emitter{nullptr} {}
        ~receiver_instance() { if(m_emitter) control<Signal>::disconnect(m_emitter, this); }

    private:
        friend class control<Signal>;
        template <typename> friend class signal::receiver;

        using tuple = typename Signal::traits::tuple_type;
        using slot  = typename Signal::traits::slot_type;
        using emitter_ptr   = emitter_instance<Signal>*;
        using signal_queue  = std::queue<tuple>;

        bool m_block; slot m_slot;
        signal_queue m_queue;
        emitter_ptr m_emitter;

        /* control only */
        void c_connect(emitter_ptr emitter) {
            m_emitter = emitter;
        }
        void c_disconnect() {
            m_emitter = nullptr;
        }
        auto c_emitter() {
            return m_emitter;
        }
        void c_pop_queue() {
            m_queue.pop();
        }
        void c_push(tuple signal_data) {
            if(!m_block) m_queue.push(signal_data);
        }
        bool c_queue_empty() {
            return m_queue.empty();
        }
        auto& c_queue_front() {
            return m_queue.front();
        }
        auto& c_slot() {
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

        template <typename F> requires valid_signature<Signal, F>
        void set_slot(F&& f) { m_slot = std::move(f); }
    };
    
    template <typename Signal>
    struct make_receiver_instance     { using type = receiver_instance<Signal>; };
    template <typename Signal>
    struct make_receiver_instance_ptr { using type = receiver_instance<Signal>*; };
}