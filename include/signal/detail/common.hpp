#pragma once
#include <type_traits>
#include <functional>

namespace signal::detail
{
    template <typename ... args>
    struct signal_signature {
        template <typename F>
        using is_invocable  = std::is_invocable<F, args ...>;
        using function_type = std::function<void(args ...)>;
        using tuple_type    = std::tuple<args ...>;

        template <typename F>
        static constexpr bool is_match = is_invocable<F>::value;
    };

    template <typename __tag, typename ... args>
    struct signal_id {
        using signature = signal_signature<args ...>;
        using slot      = signature::function_type;
        using tuple_type = signature::tuple_type;

        template <typename C>
        static constexpr auto make_slot(void (C::*member_function)(args ...), C* class_ptr) {
            return [class_ptr, member_function](args ... arr) -> void { return (class_ptr->*member_function)(arr ...); };
        }
        template <typename F> requires signature:: template is_match<F>
        static constexpr auto&& make_slot(F&& callable) {
            return std::move(callable);
        }
    };

    template <typename _signal, typename F>
    concept valid_signature = _signal::signature:: template is_match<F>;

    struct not_copyable {
        not_copyable() = default;
        not_copyable(const not_copyable&)           = delete;
        not_copyable& operator=(const not_copyable&)= delete;
    };


    template <typename ... elems> class signal_list  {};

    template <typename ... instances> class mixin : public instances ... 
    { public: mixin() : instances() ... {} };


    template <typename list> struct is_empty    : std::false_type {};
    template <> struct is_empty<signal_list<>>  : std::true_type  {};
    template <> struct is_empty<mixin<>>        : std::true_type  {};

    template <typename list, template <typename T> 
        class meta_function,  bool empty = is_empty<list>::value> 
    struct transformT;
    
    template <typename ... signals, template <typename T> class meta_function>
    struct transformT<signal_list<signals ...>, meta_function, false> {
        using type = signal_list<typename meta_function<signals>::type ...>;
    };
    template <typename ... signals, template <typename T> class meta_function>
    struct transformT<mixin<signals ...>, meta_function, false> {
        using type = mixin<typename meta_function<signals>::type ...>;
    };

    template <typename list, template <typename T> class meta_function>
    using transform = typename transformT<list, meta_function>::type;


    template <typename _signal> class control;
    template <typename _signal> class emitter_instance;
    template <typename _signal> class receiver_instance;
}