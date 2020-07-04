#pragma once
#include <type_traits>
#include <functional>

namespace signal::detail
{
    template <typename ... args>
    struct signal_traits {

        using slot_type  = std::function<void(args ...)>;
        using tuple_type = std::tuple<args ...>;

        template <typename F>
        static constexpr bool is_match = std::is_invocable<F, args ...>::value;

        template <typename F> requires is_match<F>
        static constexpr auto&& make_slot(F&& callable) {
            return std::move(callable);
        }
        template <typename C>
        static constexpr auto make_slot(void (C::*member_function)(args ...), C* class_ptr) {
            return [class_ptr, member_function](args ... arr) -> void { return (class_ptr->*member_function)(arr ...); };
        }
    };

    template <typename Signal, typename F>
    concept valid_signature = Signal::traits:: template is_match<F>;

    struct not_copyable {
        not_copyable() = default;
        not_copyable(const not_copyable&)           = delete;
        not_copyable& operator=(const not_copyable&)= delete;
    };


    template <typename ... elems> class signal_list  {};

    template <typename ... instances> 
    class mixin : public instances ... 
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


    template <typename Signal> class control;
    template <typename Signal> class emitter_instance;
    template <typename Signal> class receiver_instance;

    struct tag {};
}