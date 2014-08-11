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

			virtual void setCancelled(bool c)
			{
				cancelled = c;
			}
			virtual bool isCancelled() final
			{
				return cancelled;
			}

			virtual void react() const override
			{
				if(!cancelled)
				{
					ParentE::react();
					Registrar::react(*this);
				}
			}

		private:
			bool cancelled = false;
		};
		inline CancellableEvent::~CancellableEvent() = default;
	}
}

#endif
