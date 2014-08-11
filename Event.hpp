#ifndef resplunk_event_Event_HeaderPlusPlus
#define resplunk_event_Event_HeaderPlusPlus

#include "resplunk/event/Events.hpp"

namespace resplunk
{
	namespace event
	{
		struct Event
		: EventImplementor<Event, void>
		{
			virtual ~Event() = 0;

			virtual void process() override
			{
				return Registrar::process(*this);
			}
			virtual void react() const override
			{
				return Registrar::react(*this);
			}

			virtual void call() final
			{
				process();
				react();
			}
		};
		inline Event::~Event() = default;
	}
}

#endif
