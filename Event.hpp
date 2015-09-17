#ifndef LB_events_Event_HeaderPlusPlus
#define LB_events_Event_HeaderPlusPlus

#include "Events.hpp"

namespace LB
{
	namespace events
	{
		struct Event
		: Implementor<Event>
		{
			virtual ~Event() = 0;

			virtual void call() noexcept final
			{
				process();
				react();
			}

			virtual bool should_process(ProcessorBase const &) const noexcept
			{
				return true;
			}
			virtual bool should_react(ReactorBase const &) const noexcept
			{
				return true;
			}
		};
		inline Event::~Event() = default;
	}
}

#endif
