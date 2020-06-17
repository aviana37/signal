#pragma once
#include "common.hpp"
#include <forward_list>


namespace signal { template <typename ... signals> class emitter; }

namespace signal::detail
{
    template <typename _signal>
    class emitter_instance : not_copyable
    {
    public:
        emitter_instance() : m_receivers{} {}
        ~emitter_instance() { control<_signal>::disconnect(this); }

    private:
        friend class control<_signal>;
        template <typename> friend class signal::emitter;

        using receiver_ptr = receiver_instance<_signal>*;
        std::forward_list<receiver_ptr> m_receivers;


        /* control only */
        void __connect   (receiver_ptr receiver) { m_receivers.push_front(receiver);        }
        bool __disconnect(receiver_ptr receiver) { return m_receivers.remove(receiver) > 0; }
        void __disconnect()       { m_receivers.clear(); }
        const auto& __receivers() { return m_receivers;  }
        /* control only */

        bool is_connected(receiver_ptr receiver) { 
            return std::find(m_receivers.begin(), m_receivers.end(), receiver) != m_receivers.end(); 
        }

    };
    
    template <typename _signal>
    struct emitter_access {
        emitter_instance<_signal>* instance = nullptr;
    };

    template <typename _signal>
    struct make_emitter_instance     { using type = emitter_instance<_signal>; };
    template <typename _signal>
    struct make_emitter_instance_ptr { using type = emitter_instance<_signal>*; };
}