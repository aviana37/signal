#pragma once
#include "emitter.hpp"
#include "receiver.hpp"

namespace signal 
{
    template <typename tag, typename ... signature>
    struct tagged_id 
    {
    private: 
        using signal_id = tagged_id<tag, signature ...>;
        using traits = detail::signal_traits<signature ...>;
        template <typename F>
        static constexpr bool valid_slot = traits:: template is_match<F>;

        template <typename ...> friend class emitter;
        template <typename ...> friend class receiver;
        friend class detail::control<signal_id>;
        friend class detail::emitter_instance<signal_id>;
        friend class detail::receiver_instance<signal_id>;
    };

    template <typename ... signature>
    using id = tagged_id<detail::tag, signature ...>;
}