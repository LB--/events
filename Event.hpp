#ifndef resplunk_event_Event_HeaderPlusPlus
#define resplunk_event_Event_HeaderPlusPlus

#include "resplunk/event/Events.hpp"

namespace resplunk
{
	namespace event
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
