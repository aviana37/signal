#pragma once
#include "common.hpp"
#include <forward_list>


namespace signal 
{ 
    template <typename ... signals> 
    class emitter; 
}

namespace signal::detail
{
    template <typename Signal>
    class emitter_instance : not_copyable
    {
    public:
        emitter_instance() : m_receivers{} {}
        ~emitter_instance() { control<Signal>::disconnect(this); }

    private:
        friend class control<Signal>;
        template <typename> friend class signal::emitter;

        using receiver_ptr = receiver_instance<Signal>*;
        std::forward_list<receiver_ptr> m_receivers;


        /* control only */
        void c_connect   (receiver_ptr receiver) { m_receivers.push_front(receiver);        }
        bool c_disconnect(receiver_ptr receiver) { return m_receivers.remove(receiver) > 0; }
        void c_disconnect()       { m_receivers.clear(); }
        const auto& c_receivers() { return m_receivers;  }
        /* control only */

        bool is_connected(receiver_ptr receiver) { 
            return std::find(m_receivers.begin(), m_receivers.end(), receiver) != m_receivers.end(); 
        }

    };

    template <typename Signal>
    struct make_emitter_instance     { using type = emitter_instance<Signal>; };
    template <typename Signal>
    struct make_emitter_instance_ptr { using type = emitter_instance<Signal>*; };
}