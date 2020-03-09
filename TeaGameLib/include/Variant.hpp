#pragma once
#include <variant>
#include <tuple>

#define match(x) teaGameLib::Visit(x,[&](auto&& x)
#define match2(x1,x2) teaGameLib::Visit(x1,x2,[&](auto&& x1,auto&& x2)
#define ret_match(x) return match(x)
#define ret_match2(x1,x2) return match2(x1,x2)
#define case(x,t) if constexpr (std::is_same_v<std::decay_t<decltype(x)>, t>)
#define case_expr(x,t) case(x,t) return
#define case2(x1,x2,t1,t2) \
	if constexpr (std::is_same_v<std::decay_t<decltype(x1)>, t1> && \
	std::is_same_v<std::decay_t<decltype(x2)>, t2>)
#define case_expr2(x1,x2,t1,t2) case2(x1,x2,t1,t2) return
#define else_expr else return
#define match_end )

namespace teaGameLib {

	template<typename Tuple, std::size_t ...I>
	struct tuple_pick {
		tuple_pick(Tuple, std::index_sequence<I...>) {}
		using Type = std::tuple< std::tuple_element_t<I, Tuple>...>;
	};

	template<typename Callable, typename ...Variants>
	struct visit_helper {
		visit_helper(std::tuple<Callable, Variants...>) {};
		static auto Visit(Variants&& ...variants, Callable&& callable) {
			return std::visit(std::forward<Callable>(callable), std::forward<Variants>(variants)...);
		}
	};

	template<typename ...Args>
	using Variant = std::variant<Args...>;

	template<typename ...Args>
	auto Visit(Args&& ...args){
		using ArgsTuple = std::tuple<Args...>;
		using ArgsSize = std::tuple_size<ArgsTuple>;
		using HeadsIndexSeq = std::make_index_sequence<ArgsSize::value - 1>;
		using TailIndexSeq = std::index_sequence<ArgsSize::value - 1>;
		using HeadsTuple = typename decltype(
			tuple_pick{ std::declval<ArgsTuple>(),HeadsIndexSeq{} }
		)::Type;
		using TailTuple = typename decltype(
			tuple_pick{ std::declval<ArgsTuple>(),TailIndexSeq{} }
		)::Type;

		using LastTuple = decltype(
			std::tuple_cat(std::declval<TailTuple>(), std::declval<HeadsTuple>())
		);
		return decltype(visit_helper{ std::declval<LastTuple>() })::Visit(std::forward<Args>(args)...);
	}
}

