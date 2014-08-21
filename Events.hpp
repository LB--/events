#ifndef resplunk_event_Events_HeaderPlusPlus
#define resplunk_event_Events_HeaderPlusPlus

#include "resplunk/util/TMP.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>
#include <functional>
#include <memory>
#include <map>

namespace resplunk
{
	namespace event
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
			virtual void process(Event_t &e) const noexcept = 0;

			friend typename Event_t::Registrar_t;
		};

		template<typename EventT>
		struct LambdaProcessor
		: Processor<EventT>
		{
			using Lambda_t = std::function<void (EventT &e) /*const*/>;
			LambdaProcessor(Lambda_t l, ListenerPriority priority = ListenerPriority{}) noexcept
			: Processor<EventT>(priority)
			, lambda{l}
			{
			}
			LambdaProcessor(LambdaProcessor &&from)
			: lambda{std::move(from.lambda)}
			{
			}

		private:
			Lambda_t lambda;
			virtual void process(EventT &e) const noexcept override
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
			virtual void react(Event_t const &e) noexcept = 0;

			friend typename Event_t::Registrar_t;
		};

		template<typename EventT>
		struct LambdaReactor
		: Reactor<EventT>
		{
			using Lambda_t = std::function<void (EventT const &e)>;
			LambdaReactor(Lambda_t l, ListenerPriority priority = ListenerPriority{})
			: Reactor<EventT>(priority) noexcept
			, lambda{l}
			{
			}
			LambdaReactor(LambdaReactor &&from)
			: lambda{std::move(from.lambda)}
			{
			}

		private:
			Lambda_t lambda;
			virtual void react(EventT const &e) noexcept override
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

			static void process(Event_t &e) noexcept
			{
				for(p : processors())
				{
					if(e.should_process(p.second.get()))
					{
						p.second.get().process(e);
					}
				}
			}
			static void react(Event_t const &e) noexcept
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
				static void process(T &t) noexcept
				{
					First::Registrar_t::process(t);
					Next::process(t);
				}
				static void react(T const &t) noexcept
				{
					First::Registrar_t::react(t);
					Next::react(t);
				}
				static auto parents(T &t) noexcept
				{
					return tuple_cat(std::tuple<First &>{t}, Next::parents(t));
				}
				static auto parents(T const &t) noexcept
				{
					return tuple_cat(std::tuple<First const &>{t}, Next::parents(t));
				}
				using all_parents_t = typename util::tuple_type_cat
				<
					typename First::Unwrapper_t::all_parents_t,
					typename Rest::Unwrapper_t::all_parents_t...,
					std::tuple<First>,
					typename Next::all_parents_t
				>::type;
			};
			template<typename T>
			struct Unwrapper<T> final
			{
				static_assert(std::is_base_of<Event, typename T::Event_t>::value, "Only Event can be root");
				Unwrapper() = delete;
				static void process(T &t) noexcept
				{
					T::Registrar_t::process(dynamic_cast<typename T::Event_t &>(t));
				}
				static void react(T const &t) noexcept
				{
					T::Registrar_t::react(dynamic_cast<typename T::Event_t const &>(t));
				}
				static auto parents(T &t) noexcept
				-> std::tuple<>
				{
					return {};
				}
				static auto parents(T const &t) noexcept
				-> std::tuple<>
				{
					return {};
				}
				using all_parents_t = std::tuple<>;
			};
			struct PR
			{
				virtual ~PR() noexcept = default;
				virtual void process() noexcept = 0;
				virtual void react() const noexcept = 0;
			};
		}
		template<typename EventT, typename... ParentT>
		struct Implementor
		: virtual impl::PR
		, virtual ParentT...
		{
			using Unwrapper_t = impl::Unwrapper<Implementor, ParentT...>;
			using Event_t = EventT;
			using Parents_t = std::tuple<ParentT &...>;
			using ConstParents_t = std::tuple<ParentT const &...>;
			using Implementor_t = Implementor;
			using Processor_t = Processor<EventT>;
			using Reactor_t = Reactor<EventT>;
			using Registrar_t = Registrar<EventT>;
			static constexpr bool MI = (sizeof...(ParentT) > 1);
			static constexpr bool ROOT = (sizeof...(ParentT) == 0);
			virtual ~Implementor() = 0;

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

			virtual void process() noexcept override
			{
				util::tuple_template_forward
				<
					impl::Unwrapper,
					typename util::tuple_type_cat
					<
						std::tuple<Implementor_t>,
						typename util::tuple_prune
						<
							typename Unwrapper_t::all_parents_t
						>::type
					>::type
				>::type::process(*this);
			}
			virtual void react() const noexcept override
			{
				util::tuple_template_forward
				<
					impl::Unwrapper,
					typename util::tuple_type_cat
					<
						std::tuple<Implementor_t>,
						typename util::tuple_prune
						<
							typename Unwrapper_t::all_parents_t
						>::type
					>::type
				>::type::react(*this);
			}

		private:
			friend Event_t;
			Implementor() = default;

			friend Registrar_t;
			static Registrar_t &registrar() noexcept;
		};
		template<typename EventT, typename... ParentT>
		Implementor<EventT, ParentT...>::~Implementor<EventT, ParentT...>() = default;

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

#define RESPLUNK_EVENT(n) \
	template<> \
	auto ::n::Implementor_t::registrar() noexcept \
	-> Registrar_t & \
	{ \
		static Registrar_t r;\
		return r;\
	}

#endif
