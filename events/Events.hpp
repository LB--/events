#ifndef LB_events_Events_HeaderPlusPlus
#define LB_events_Events_HeaderPlusPlus

#include "LB/tuples/tuples.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>
#include <functional>
#include <memory>
#include <map>

namespace LB
{
	namespace events
	{
		struct Event;

		struct ListenerPriority final
		{
			using Priority_t = std::intmax_t;

			static constexpr Priority_t FIRST = std::numeric_limits<Priority_t>::min();
			static constexpr Priority_t LAST  = std::numeric_limits<Priority_t>::max();

			constexpr ListenerPriority() noexcept
			: priority{}
			{
			}
			constexpr ListenerPriority(Priority_t p) noexcept
			: priority{p}
			{
			}
			constexpr ListenerPriority(ListenerPriority const &) = default;
			ListenerPriority &operator=(ListenerPriority const &) = delete;
			constexpr ListenerPriority(ListenerPriority &&) = default;
			ListenerPriority &operator=(ListenerPriority &&) = delete;

			constexpr operator Priority_t() const noexcept
			{
				return priority;
			}

			friend constexpr bool operator==(ListenerPriority const &a, ListenerPriority const &b) noexcept
			{
				return a.priority == b.priority;
			}
			friend constexpr bool operator<(ListenerPriority const &a, ListenerPriority const &b) noexcept
			{
				return a.priority < b.priority;
			}

		private:
			Priority_t const priority;
		};

		struct ProcessorBase
		{
			virtual ~ProcessorBase() = 0;
		};
		inline ProcessorBase::~ProcessorBase() = default;
		template<typename EventT>
		struct Processor
		: virtual ProcessorBase
		{
			static_assert(std::is_base_of<Event, EventT>::value, "EventT must derive from Event");
			using Event_t = EventT;
			Processor(ListenerPriority priority = ListenerPriority{}) noexcept
			{
				listen(priority);
			}
			Processor(Processor const &) = delete;
			Processor &operator=(Processor const &) = delete;
			Processor(Processor &&) = delete;
			Processor &operator=(Processor &&) = delete;
			virtual ~Processor() noexcept
			{
				ignore();
			}

		protected:
			void listen(ListenerPriority priority = ListenerPriority{}) noexcept
			{
				return Event_t::listen(*this, priority);
			}
			void ignore() noexcept
			{
				return Event_t::ignore(*this);
			}

		private:
			virtual void process(Event_t &e) const noexcept(EventT::NOEXCEPT) = 0;

			friend typename Event_t::Registrar_t;
		};

		template<typename EventT>
		struct LambdaProcessor final
		: Processor<EventT>
		{
			using Lambda_t = std::function<void (EventT &e) /*const*/>;
			LambdaProcessor(Lambda_t l, ListenerPriority priority = ListenerPriority{}) noexcept
			: Processor<EventT>(priority)
			, lambda{l}
			{
			}
			LambdaProcessor(LambdaProcessor const &from) noexcept
			: lambda{from.lambda}
			{
			}
			LambdaProcessor &operator=(LambdaProcessor const &from) noexcept
			{
				lambda = from.lambda;
				return *this;
			}
			LambdaProcessor(LambdaProcessor &&from) noexcept
			: lambda{std::move(from.lambda)}
			{
			}
			LambdaProcessor &operator=(LambdaProcessor &&from) noexcept
			{
				lambda = std::move(from.lambda);
				return *this;
			}

		private:
			Lambda_t lambda;
			virtual void process(EventT &e) const noexcept(EventT::NOEXCEPT) override
			{
				return lambda(e);
			}
		};

		struct ReactorBase
		{
			virtual ~ReactorBase() = 0;
		};
		inline ReactorBase::~ReactorBase() = default;
		template<typename EventT>
		struct Reactor
		: virtual ReactorBase
		{
			static_assert(std::is_base_of<Event, EventT>::value, "EventT must derive from Event");
			using Event_t = EventT;
			Reactor(ListenerPriority priority = ListenerPriority{}) noexcept
			{
				listen(priority);
			}
			Reactor(Reactor const &) = delete;
			Reactor &operator=(Reactor const &) = delete;
			Reactor(Reactor &&) = delete;
			Reactor &operator=(Reactor &&) = delete;
			virtual ~Reactor() noexcept
			{
				ignore();
			}

		protected:
			void listen(ListenerPriority priority = ListenerPriority{}) noexcept
			{
				return Event_t::listen(*this, priority);
			}
			void ignore() noexcept
			{
				return Event_t::ignore(*this);
			}

		private:
			virtual void react(Event_t const &e) noexcept(EventT::NOEXCEPT) = 0;

			friend typename Event_t::Registrar_t;
		};

		template<typename EventT>
		struct LambdaReactor
		: Reactor<EventT>
		{
			using Lambda_t = std::function<void (EventT const &e)>;
			LambdaReactor(Lambda_t l, ListenerPriority priority = ListenerPriority{}) noexcept
			: Reactor<EventT>(priority)
			, lambda{l}
			{
			}
			LambdaReactor(LambdaReactor const &from) noexcept
			: lambda{from.lambda}
			{
			}
			LambdaReactor &operator=(LambdaReactor const &from) noexcept
			{
				lambda = from.lambda;
				return *this;
			}
			LambdaReactor(LambdaReactor &&from) noexcept
			: lambda{std::move(from.lambda)}
			{
			}
			LambdaReactor &operator=(LambdaReactor &&from) noexcept
			{
				lambda = std::move(from.lambda);
				return *this;
			}

		private:
			Lambda_t lambda;
			virtual void react(EventT const &e) noexcept(EventT::NOEXCEPT) override
			{
				return lambda(e);
			}
		};

		template<typename EventT>
		struct Registrar final
		{
			static_assert(std::is_base_of<Event, EventT>::value, "EventT must derive from Event");
			using Event_t = EventT;
			using Processor_t = Processor<EventT>;
			using Reactor_t = Reactor<EventT>;

			static void listen(Processor_t const &p, ListenerPriority priority = ListenerPriority{}) noexcept
			{
				ignore(p);
				processors().emplace(priority, std::cref(p));
			}
			static void listen(Reactor_t &r, ListenerPriority priority = ListenerPriority{}) noexcept
			{
				ignore(r);
				reactors().emplace(priority, std::ref(r));
			}

			static void ignore(Processor_t const &p) noexcept
			{
				auto &procs = processors();
				for(auto it = procs.begin(); it != procs.end(); )
				{
					if(std::addressof(p) == std::addressof(it->second.get()))
					{
						it = procs.erase(it);
					}
					else ++it;
				}
			}
			static void ignore(Reactor_t &r) noexcept
			{
				auto &reacts = reactors();
				for(auto it = reacts.begin(); it != reacts.end(); )
				{
					if(std::addressof(r) == std::addressof(it->second.get()))
					{
						it = reacts.erase(it);
					}
					else ++it;
				}
			}

			static void process(Event_t &e) noexcept(EventT::NOEXCEPT)
			{
				for(p : processors())
				{
					if(e.should_process(p.second.get()))
					{
						p.second.get().process(e);
					}
				}
			}
			static void react(Event_t const &e) noexcept(EventT::NOEXCEPT)
			{
				for(r : reactors())
				{
					if(e.should_react(r.second.get()))
					{
						r.second.get().react(e);
					}
				}
			}

		private:
			friend typename Event_t::Implementor_t;
			Registrar() = default;

			using Processors_t = std::multimap<ListenerPriority, std::reference_wrapper<Processor_t const>>;
			using Reactors_t = std::multimap<ListenerPriority, std::reference_wrapper<Reactor_t>>;
			Processors_t ps;
			Reactors_t rs;
			static auto processors() noexcept
			-> Processors_t &
			{
				return Event_t::registrar().ps;
			}
			static auto reactors() noexcept
			-> Reactors_t &
			{
				return Event_t::registrar().rs;
			}
		};

		namespace impl
		{
			template<typename T, typename...>
			struct Unwrapper;
			template<typename T, typename First, typename... Rest>
			struct Unwrapper<T, First, Rest...> final
			{
				static_assert(std::is_base_of<Event, First>::value, "ParentT must derive from Event");
				using Next = Unwrapper<T, Rest...>;
				Unwrapper() = delete;
				static void process(T &t) noexcept(T::NOEXCEPT)
				{
					First::Registrar_t::process(t);
					Next::process(t);
				}
				static void react(T const &t) noexcept(T::NOEXCEPT)
				{
					First::Registrar_t::react(t);
					Next::react(t);
				}
				static auto parents(T &t) noexcept
				{
					return tuple_cat(tuples::tuple<First &>{t}, Next::parents(t));
				}
				static auto parents(T const &t) noexcept
				{
					return tuple_cat(tuples::tuple<First const &>{t}, Next::parents(t));
				}
				using all_parents_t = typename tuples::tuple_type_cat
				<
					typename First::Unwrapper_t::all_parents_t,
					typename Rest::Unwrapper_t::all_parents_t...,
					tuples::tuple<First>,
					typename Next::all_parents_t
				>::type;
			};
			template<typename T>
			struct Unwrapper<T> final
			{
				static_assert(std::is_base_of<Event, typename T::Event_t>::value, "Only Event can be root");
				Unwrapper() = delete;
				static void process(T &t) noexcept(T::NOEXCEPT)
				{
					T::Registrar_t::process(dynamic_cast<typename T::Event_t &>(t));
				}
				static void react(T const &t) noexcept(T::NOEXCEPT)
				{
					T::Registrar_t::react(dynamic_cast<typename T::Event_t const &>(t));
				}
				static auto parents(T &t) noexcept
				-> tuples::tuple<>
				{
					return {};
				}
				static auto parents(T const &t) noexcept
				-> tuples::tuple<>
				{
					return {};
				}
				using all_parents_t = tuples::tuple<>;
			};
			template<typename... ParentT>
			struct is_noexcept final
			: std::false_type
			{
			};
			template<typename First, typename... Rest>
			struct is_noexcept<First, Rest...> final
			: std::integral_constant<bool, First::NOEXCEPT || is_noexcept<Rest...>::value> //std::bool_constant
			{
			};
			template<typename... ParentT>
			struct Inheriter
			: virtual ParentT...
			{
				static constexpr bool NOEXCEPT = is_noexcept<ParentT...>::value;
				virtual ~Inheriter() noexcept = default;

				virtual void process() noexcept(NOEXCEPT) override = 0;
				virtual void react() const noexcept(NOEXCEPT) override = 0;
			};
			template<>
			struct Inheriter<>
			{
				static constexpr bool NOEXCEPT = false;
				virtual ~Inheriter() noexcept = default;

				virtual void process() noexcept(NOEXCEPT) = 0;
				virtual void react() const noexcept(NOEXCEPT) = 0;
			};
		}
		template<typename EventT, typename... ParentT>
		struct Implementor;
		template<typename EventT, template<typename...> typename InheriterT, typename... ParentT>
		struct Implementor<EventT, InheriterT<ParentT...>>
		: virtual InheriterT<ParentT...>
		{
			using Unwrapper_t = impl::Unwrapper<Implementor, ParentT...>;
			using Event_t = EventT;
			using Inheriter_t = InheriterT<ParentT...>;
			using Parents_t = tuples::tuple<ParentT &...>;
			using ConstParents_t = tuples::tuple<ParentT const &...>;
			using Implementor_t = Implementor;
			using Processor_t = Processor<EventT>;
			using Reactor_t = Reactor<EventT>;
			using Registrar_t = Registrar<EventT>;
			static constexpr bool MI = (sizeof...(ParentT) > 1);
			static constexpr bool ROOT = (sizeof...(ParentT) == 0);
			static constexpr bool NOEXCEPT = Inheriter_t::NOEXCEPT;
			virtual ~Implementor() noexcept = 0;

			static void listen(Processor_t const &p, ListenerPriority priority = ListenerPriority{}) noexcept
			{
				return Registrar_t::listen(p, priority);
			}
			static void listen(Reactor_t &r, ListenerPriority priority = ListenerPriority{}) noexcept
			{
				return Registrar_t::listen(r, priority);
			}

			static void ignore(Processor_t const &p) noexcept
			{
				return Registrar_t::ignore(p);
			}
			static void ignore(Reactor_t &r) noexcept
			{
				return Registrar_t::ignore(r);
			}

			virtual void process() noexcept(NOEXCEPT) override
			{
				tuples::tuple_template_forward
				<
					impl::Unwrapper,
					typename tuples::tuple_type_cat
					<
						tuples::tuple<Implementor_t>,
						typename tuples::tuple_prune
						<
							typename Unwrapper_t::all_parents_t
						>::type
					>::type
				>::type::process(*this);
			}
			virtual void react() const noexcept(NOEXCEPT) override
			{
				tuples::tuple_template_forward
				<
					impl::Unwrapper,
					typename tuples::tuple_type_cat
					<
						tuples::tuple<Implementor_t>,
						typename tuples::tuple_prune
						<
							typename Unwrapper_t::all_parents_t
						>::type
					>::type
				>::type::react(*this);
			}

		private:
			friend Event_t;
			friend Implementor<EventT, ParentT...>;
			Implementor() noexcept = default;

			friend Registrar_t;
			static Registrar_t &registrar() noexcept;
		};
		template<typename EventT, template<typename...> typename InheriterT, typename... ParentT>
		Implementor<EventT, InheriterT<ParentT...>>::~Implementor<EventT, InheriterT<ParentT...>>() noexcept = default;
		template<typename EventT, typename... ParentT>
		struct Implementor
		: Implementor<EventT, impl::Inheriter<ParentT...>>
		{
			using typename Implementor<EventT, impl::Inheriter<ParentT...>>::Implementor_t;
			virtual ~Implementor() noexcept = 0;

		private:
			friend typename Implementor_t::Event_t;
			Implementor() noexcept = default;
		};
		template<typename EventT, typename... ParentT>
		Implementor<EventT, ParentT...>::~Implementor<EventT, ParentT...>() noexcept = default;

		template<typename EventT, typename... ParentT>
		auto parents(Implementor<EventT, ParentT...> &e) noexcept
		-> typename std::remove_reference<decltype(e)>::type::Parents_t
		{
			return std::remove_reference<decltype(e)>::type::Unwrapper_t::parents(e);
		}
		template<typename EventT, typename... ParentT>
		auto parents(Implementor<EventT, ParentT...> const &e) noexcept
		-> typename std::remove_reference<decltype(e)>::type::ConstParents_t
		{
			return std::remove_reference<decltype(e)>::type::Unwrapper_t::parents(e);
		}
	}
}

//Necessary evil is necessary
#define LB_EVENTS_EVENT(E) \
	namespace LB \
	{ \
		namespace events \
		{ \
			template<> \
			auto E::Implementor_t::registrar() noexcept \
			-> Registrar_t & \
			{ \
				static Registrar_t r; \
				return r; \
			} \
		} \
	} \
//https://github.com/LB--/events/#the-ugly-part

#endif
