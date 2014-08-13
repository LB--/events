#ifndef resplunk_event_Cancellable_HeaderPlusPlus
#define resplunk_event_Cancellable_HeaderPlusPlus

#include "resplunk/event/Event.hpp"

namespace resplunk
{
	namespace event
	{
		struct Cancellable
		: Implementor<Cancellable, Event>
		{
			virtual ~Cancellable() = 0;

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
		inline Cancellable::~Cancellable() = default;
	}
}

#endif
