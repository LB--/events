#ifndef resplunk_event_Event_HeaderPlusPlus
#define resplunk_event_Event_HeaderPlusPlus

#include "resplunk/event/Events.hpp"

namespace resplunk
{
	namespace event
	{
		struct Event
		: EventImplementor<Event>
		{
			virtual ~Event() = 0;

			virtual void call() final
			{
				process();
				react();
			}

			virtual bool shouldProcess() const
			{
				return true;
			}
			virtual bool shouldReact() const
			{
				return true;
			}
		};
		inline Event::~Event() = default;
	}
}

#endif
