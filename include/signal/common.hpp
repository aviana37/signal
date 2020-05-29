#pragma once
#include <type_traits>
#include <functional>

namespace signal 
{
    namespace detail 
    {
        template <typename ... args>
        struct signal_signature {
            template <typename F>
            using is_invocable = std::is_invocable<F, args ...>;
            using function_type = std::function<void(args ...)>;
            using tuple_type = std::tuple<args ...>;

            template <typename F>
            static constexpr bool is_match = is_invocable<F>::value;
        };

        template <typename __id_tag, typename ... args>
        struct signal_id {
            using signature = signal_signature<args ...>;
            using slot = std::function<void(args ...)>;
            using tuple_type = std::tuple<args ...>;
        };

        template <typename _signal, typename F>
        concept valid_signature = _signal::signature:: template is_match<F>;

        template <typename ... elems> class signal_list  {};
        template <typename ... elems> class signal_mixin : public elems ... {
            public: signal_mixin() : elems() ... {}
        };

        template <typename list> struct is_empty : std::false_type {};
        template<> struct is_empty<signal_list<>> : std::true_type {};
        template<> struct is_empty<signal_mixin<>>: std::true_type {};

        template <typename list, template <typename T> class meta_function, bool empty = is_empty<list>::value>
        struct transformT;
        template <typename ... signals, template <typename T> class meta_function>
        struct transformT<signal_list<signals ...>, meta_function, false> {
            using type = signal_list<typename meta_function<signals>::type ...>;
        };
        template <typename ... signals, template <typename T> class meta_function>
        struct transformT<signal_mixin<signals ...>, meta_function, false> {
            using type = signal_mixin<typename meta_function<signals>::type ...>;
        };
        template <typename list, template <typename T> class meta_function>
        using transform = typename transformT<list, meta_function>::type;

        template <typename S> class emitter_instance;
        template <typename S> class emitter_access;
        template <typename S> class receiver_instance;
        template <typename S> class receiver_access;
    }
    namespace __tags {}    

    template <typename S> class receiver_access;

    template <typename _signal, typename F>
    requires detail::valid_signature<_signal, F>
    void connect(detail::emitter_instance<_signal>& emitter, detail::receiver_instance<_signal>& receiver, F&& slot);
    template <typename _signal> void disconnect(detail::emitter_instance<_signal>& emitter);
    template <typename _signal> void disconnect(detail::receiver_instance<_signal>& receiver);
    template <typename _signal> void disconnect(detail::emitter_instance <_signal>& emitter, 
                                                detail::receiver_instance<_signal>& receiver);
    template <typename _signal>
    void emit(detail::emitter_instance<_signal>& emitter, typename _signal::tuple_type signal_data);
    template <typename _signal>
    void receive(detail::emitter_instance<_signal>& receiver);
}