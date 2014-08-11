#ifndef resplunk_event_Events_HeaderPlusPlus
#define resplunk_event_Events_HeaderPlusPlus

#include "resplunk/server/ServerSpecific.hpp"
#include "resplunk/util/TemplateImplRepo.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>
#include <functional>
#include <memory>
#include <map>
#include <exception>

namespace resplunk
{
	namespace event
	{
		struct Event;

		struct ListenerPriority final
		{
			using Priority_t = std::intmax_t;

			static constexpr ListenerPriority FIRST = std::numeric_limits<Priority_t>::min();
			static constexpr ListenerPriority LAST  = std::numeric_limits<Priority_t>::max();

			constexpr ListenerPriority() = default;
			constexpr ListenerPriority(Priority_t p)
			: priority{p}
			{
			}
			constexpr ListenerPriority(ListenerPriority const &) = default;
			ListenerPriority &operator=(ListenerPriority const &) = delete;
			constexpr ListenerPriority(ListenerPriority &&) = default;
			ListenerPriority &operator=(ListenerPriority &&) = delete;
			constexpr ~ListenerPriority() = default;

			constexpr operator Priorty_t() const
			{
				return priority;
			}

			friend constexpr bool operator==(ListenerPriority const &a, ListenerPriority const &b)
			{
				return a.priority == b.priority;
			}
			friend constexpr bool operator<(ListenerPriority const &a, ListenerPriority const &b)
			{
				return a.priority < b.priority;
			}

		private:
			Priority_t const priority;
		};

		template<typename EventT>
		struct EventProcessor
		: virtual server::ServerSpecific
		{
			static_assert(std::is_base_of<Event, EventT>::value, "EventT must derive from Event");
			using E = EventT;
			EventProcessor() = default;
			EventProcessor(ListenerPriority priority)
			{
				listen(priority);
			}
			EventProcessor(EventProcessor const &) = delete;
			EventProcessor &operator=(EventProcessor const &) = delete;
			EventProcessor(EventProcessor &&) = delete;
			EventProcessor &operator=(EventProcessor &&) = delete;
			virtual ~EventProcessor()
			{
				ignore();
			}

		protected:
			virtual void listen(ListenerPriority priority = ListenerPriority{}) final
			{
				return E::listen(*this, priority);
			}
			virtual void ignore() final
			{
				return E::ignore(*this);
			}

		private:
			virtual void onEvent(E &e) const = 0;

			friend typename E::Registrar;
		};

		template<typename EventT>
		struct LambdaEventProcessor
		: EventProcessor<EventT>
		{
			using Lambda_t = std::function<void (E &e) const>;
		protected:
			LambdaEventProcessor(Lambda_t l)
			: lambda(l)
			{
			}

			virtual void onEvent(E &e) const override
			{
				return lambda(e);
			}

		private:
			Lambda_t lambda;
		};

		template<typename EventT>
		struct EventReactor
		: virtual server::ServerSpecific
		{
			static_assert(std::is_base_of<Event, EventT>::value, "EventT must derive from Event");
			using E = EventT;
			EventReactor() = default;
			EventReactor(ListenerPriority priority)
			{
				listen(priority);
			}
			EventReactor(EventReactor const &) = delete;
			EventReactor &operator=(EventReactor const &) = delete;
			EventReactor(EventReactor &&) = delete;
			EventReactor &operator=(EventReactor &&) = delete;
			virtual ~EventReactor()
			{
				ignore();
			}

		protected:
			virtual void listen(ListenerPriority priority = ListenerPriority{}) final
			{
				return E::listen(*this, priority);
			}
			virtual void ignore() final
			{
				return E::ignore(*this);
			}

		private:
			virtual void onEvent(E const &e) = 0;

			friend typename E::Registrar;
		};

		template<typename EventT>
		struct LambdaEventReactor
		: EventReactor<EventT>
		{
			using Lambda_t = std::function<void (E const &e)>;
		protected:
			LambdaEventReactor(Lambda_t l)
			: lambda(l)
			{
			}

			virtual void onEvent(E const &e) override
			{
				return lambda(e);
			}

		private:
			Lambda_t lambda;
		};

		template<typename T>
		struct DestructEvent;

		template<typename EventT>
		struct EventRegistrar final
		{
			static_assert(std::is_base_of<Event, EventT>::value, "EventT must derive from Event");
			using E = EventT;
			using Processor = EventProcessor<EventT>;
			using Reactor = EventReactor<EventT>;
			EventRegistrar() = delete;
			EventRegistrar(EventRegistrar const &) = delete;
			EventRegistrar(EventRegistrar &&) = delete;

			static void listen(Processor const &p, ListenerPriority priority = ListenerPriority{})
			{
				processors(p.server()).emplace(priority, std::cref(p));
			}
			static void listen(Reactor &r, ListenerPriority priority = ListenerPriority{})
			{
				reactors(r.server()).emplace(priority, std::ref(r));
			}

			static void ignore(Processor const &p)
			{
				auto &procs = processors(p.server());
				for(auto it = procs.begin(); it != procs.end(); )
				{
					if(std::addressof(p) == std::addressof(it->second.get()))
					{
						it = procs.erase(it);
					}
					else ++it;
				}
			}
			static void ignore(Reactor &r)
			{
				auto &reacts = reactors(p.server());
				for(auto it = reacts.begin(); it != reacts.end(); )
				{
					if(std::addressof(p) == std::addressof(it->second.get()))
					{
						it = reacts.erase(it);
					}
					else ++it;
				}
			}

			static void process(E &e)
			{
				auto &procs = processors(e.server());
				for(auto it = procs.begin(); it != procs.end(); ++it)
				{
					try
					{
						it->second.get().onEvent(e);
					}
					catch(std::exception &e)
					{
						//
					}
					catch(...)
					{
						//
					}
				}
			}
			static void react(E const &e)
			{
				auto &reacts = reactors(p.server());
				for(auto it = reacts.begin(); it != reacts.end(); ++it)
				{
					try
					{
						it->second.get().onEvent(e);
					}
					catch(std::exception &e)
					{
						//
					}
					catch(...)
					{
						//
					}
				}
			}

		private:
			struct Key final {};
			using Processors_t = std::map<Server const *, std::multimap<ListenerPriority, std::reference_wrapper<Processor const>>>;
			using Reactors_t = std::map<Server const *, std::multimap<ListenerPriority, std::reference_wrapper<Reactor>>>;
			using SDReactors_t = std::map<Server const *, std::unique_ptr<ServerDestructReactor>>;
			static Processors_t &processors()
			{
				return TemplateImplRepo::get<Key, Processors_t>();
			}
			static Reactors_t &reactors()
			{
				return TemplateImplRepo::get<Key, Reactors_t>();
			}
			static ServerDestructReactors_t &sdreactors()
			{
				return TemplateImplRepo::get<Key, SDReactors_t>();
			}
			static void sdreactors(Server &s)
			{
				if(sdreactors().find(&s) == sdreactors().end())
				{
					sdreactors().emplace
					(
						SDReactors_t::key_type{&s},
						SDReactors_t::mapped_type{new ServerDestructReactor{s}}
					);
				}
				else if(!sdreactors()[&s])
				{
					sdreactors().erase(&s);
				}
			}
			static Processors_t::mapped_type &processors(Server &s)
			{
				if(processors().find(&s) == processors().end())
				{
					sdreactors(s);
				}
				return processors()[&s];
			}
			static Reactors_t::mapped_type &reactors(Server &s)
			{
				if(reactors().find(&s) == reactors().end())
				{
					sdreactors(s);
				}
				return reactors()[&s];
			}
			static Processors_t::mapped_type &processors(Server const &s)
			{
				auto it = processors().find(&s);
				if(it != processors().end())
				{
					return it->second;
				}
				return {};
			}
			static Reactors_t::mapped_type &reactors(Server const &s)
			{
				auto it = reactors().find(&s);
				if(it != reactors().end())
				{
					return it->second;
				}
				return {};
			}
			struct ServerDestructReactor final
			: private EventReactor<DestructEvent<Server>>
			{
				ServerDestructReactor(Server &s)
				: ServerSpecific(s)
				, EventReactor<DestructEvent<Server>>(ListenerPriority::LAST)
				{
				}

			private:
				virtual void onEvent(E const &e) override
				{
					auto This = std::move(sdreactors()[&server()]);
					processors().erase(&e.instance());
					reactors().erase(&e.instance());
				}
			};
		};

		template<typename EventT, typename ParentT>
		struct EventImplementor
		: virtual ParentT
		{
			static_assert(std::is_base_of<Event, EventT>::value, "EventT must derive from Event");
			static_assert(std::is_base_of<Event, ParentT>::value, "ParentT must derive from Event");
			static_assert(std::is_base_of<EventT, ParentT>::value, "EventT must derive from ParentT");
			using E = EventT;
			using ParentE = ParentT;
			using Implementor = EventImplementor;
			using Processor = EventProcessor<EventT>;
			using Reactor = EventReactor<EventT>;
			using Registrar = EventRegistrar<EventT>;
			EventImplementor() = default;
			virtual ~EventImplementor() = 0;

			static void listen(Processor const &p, ListenerPriority priority = ListenerPriority{})
			{
				return Registrar::listen(p, priority);
			}
			static void listen(Reactor &r, ListenerPriority priority = ListenerPriority{})
			{
				return Registrar::listen(r, priority);
			}

			static void ignore(Processor const &p)
			{
				return Registrar::ignore(p);
			}
			static void ignore(Reactor &r)
			{
				return Registrar::ignore(r);
			}

			virtual void process() override
			{
				ParentE::process();
				return Registrar::process(*this);
			}
			virtual void react() const override
			{
				ParentE::react();
				return Registrar::react(*this);
			}
		};
		template<typename EventT, typename ParentT>
		EventImplementor::~EventImplementor() = default;

		template<typename EventT>
		struct EventImplementor<EventT, void>
		: virtual server::ServerSpecific
		{
			static_assert(std::is_same<EventT, Event>::value, "This can only be derived by Event");
			using E = EventT;
			using ParentE = void;
			using Implementor = EventImplementor;
			using Processor = EventProcessor<EventT>;
			using Reactor = EventReactor<EventT>;
			using Registrar = EventRegistrar<EventT>;
			EventImplementor() = default;
			EventImplementor(EventImplementor const &) = delete;
			EventImplementor &operator=(EventImplementor const &) = delete;
			EventImplementor(EventImplementor &&) = delete;
			EventImplementor &operator=(EventImplementor &&) = delete;
			virtual ~EventImplementor() = 0;

			static void listen(Processor const &p, ListenerPriority priority = ListenerPriority{})
			{
				return Registrar::listen(p, priority);
			}
			static void listen(Reactor &r, ListenerPriority priority = ListenerPriority{})
			{
				return Registrar::listen(r, priority);
			}

			static void ignore(Processor const &p)
			{
				return Registrar::ignore(p);
			}
			static void ignore(Reactor &r)
			{
				return Registrar::ignore(r);
			}

			virtual void process()
			{
				return Registrar::process(*this);
			}
			virtual void react() const
			{
				return Registrar::react(*this);
			}
		};
		template<typename EventT>
		EventImplementor<EventT, void>::~EventImplementor<EventT, void>() = default;
	}
}

#endif
