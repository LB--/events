#undef NDEBUG

#include "Event.hpp"

#include <cassert>
#include <vector>
#include <utility>

template<typename... Args>
using EI = LB::event::Implementor<Args...>;
template<typename... Args>
using LEP = LB::event::LambdaProcessor<Args...>;
template<typename... Args>
using LER = LB::event::LambdaReactor<Args...>;
using E = LB::event::Event;

struct E0 : EI<E0, E> {}; LB_EVENTS_EVENT(E0);

struct E1 : EI<E1, E0> {}; LB_EVENTS_EVENT(E1);
struct E2 : EI<E2, E0> {}; LB_EVENTS_EVENT(E2);
struct E3 : EI<E3, E0> {}; LB_EVENTS_EVENT(E3);
struct E4 : EI<E4, E0> {}; LB_EVENTS_EVENT(E4);

struct E5 : EI<E5, E1, E2> {}; LB_EVENTS_EVENT(E5);
struct E6 : EI<E6, E3, E4> {}; LB_EVENTS_EVENT(E6);

struct E7 : EI<E7, E5, E6> {}; LB_EVENTS_EVENT(E7);

std::vector<int> o {0, 1, 2, 3, 4, 5, 6, 7};
std::vector<int> po {};
std::vector<int> ro {};

template<typename T>
auto lep(int n) noexcept
-> std::pair<LEP<T>, LER<T>>
{
	return
	{
		{[&,n](T       &e){ po.push_back(n); }},
		{[&,n](T const &e){ ro.push_back(n); }}
	};
}

int main() noexcept
{
	auto l0 = lep<E0>(0);
	auto l1 = lep<E1>(1);
	auto l2 = lep<E2>(2);
	auto l3 = lep<E3>(3);
	auto l4 = lep<E4>(4);
	auto l5 = lep<E5>(5);
	auto l6 = lep<E6>(6);
	auto l7 = lep<E7>(7);
	E7{}.call();
	assert(po == o);
	assert(po == ro);
}
