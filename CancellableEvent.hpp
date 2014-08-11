#ifndef resplunk_event_CancellableEvent_HeaderPlusPlus
#define resplunk_event_CancellableEvent_HeaderPlusPlus

#include "resplunk/event/Event.hpp"

namespace resplunk
{
	namespace event
	{
		struct CancellableEvent
		: EventImplementor<CancellableEvent, Event>
		{
			virtual ~CancellableEvent() = 0;

			virtual void cancelled(bool c)
			{
				cancel = c;
			}
			virtual bool cancelled() const final
			{
				return cancel;
			}

			virtual bool shouldReact() const override
			{
				return !cancel;
			}

		private:
			bool cancel = false;
		};
		inline CancellableEvent::~CancellableEvent() = default;
	}
}

#endif
